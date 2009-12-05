# $Id$

from hqcommon import (
	blendWeights, computeLiteWeightCells, computeNeighbours, computeOffsets,
	computeWeights, computeWeightCells, makeLite as commonMakeLite,
	permuteCase, permuteCases, printSubExpr, printText,
	transformOffsets, transformWeights, writeBinaryFile, writeTextFile
	)

from copy import deepcopy
from itertools import izip

class Parser(object):

	def __init__(self):
		self.fileName = 'HQ3xScaler.in'
		self.pixelExpr = [ [ None ] * 8 for _ in range(1 << 12) ]
		self.__parse()
		sanityCheck(self.pixelExpr)

	@staticmethod
	def __filterSwitch(stream):
		log = False
		inIf = False
		for line in stream:
			line = line.strip()
			if line == 'switch (pattern) {':
				log = True
			elif line == '}':
				if inIf:
					inIf = False
				elif log:
					break
			elif line.startswith('//pixel'):
				line = line[2 : ]
			elif line.startswith('if'):
				inIf = True
			if log:
				yield line

	def __parse(self):
		cases = []
		subCases = range(1 << 4)
		for line in self.__filterSwitch(file(self.fileName)):
			if line.startswith('case'):
				cases.append(int(line[5 : line.index(':', 5)]))
			elif line.startswith('pixel'):
				subPixel = int(line[5]) - 1
				assert 0 <= subPixel < 9
				assert subPixel != (5 - 1)
				expr = line[line.index('=') + 1 : ].strip()
				self.__addCases(cases, subCases, subPixel, expr[ : -1])
			elif line.startswith('if'):
				index = line.find('edge')
				assert index != -1
				index1 = line.index('(', index)
				index2 = line.index(',', index1)
				index3 = line.index(')', index2)
				pix1s = line[index1 + 1 : index2].strip()
				pix2s = line[index2 + 1 : index3].strip()
				assert pix1s[0] == 'c', pix1s
				assert pix2s[0] == 'c', pix2s
				pix1 = int(pix1s[1:])
				pix2 = int(pix2s[1:])
				if pix1 == 2 and pix2 == 6:
					subCase = 0
				elif pix1 == 6 and pix2 == 8:
					subCase = 1
				elif pix1 == 8 and pix2 == 4:
					subCase = 2
				elif pix1 == 4 and pix2 == 2:
					subCase = 3
				else:
					assert False, (line, pix1, pix2)
				subCases = [ x for x in range(1 << 4) if x & (1 << subCase) ]
			elif line.startswith('} else'):
				subCases = [ x for x in range(1 << 4) if x not in subCases ]
			elif line.startswith('}'):
				subCases = range(1 << 4)
			elif line.startswith('break'):
				cases = []
				subCases = range(1 << 4)

	def __addCases(self, cases, subCases, subPixel, expr):
		pixelExpr = self.pixelExpr
		if subPixel > (5 - 1):
			subPixel -= 1
		for case in cases:
			for subCase in subCases:
				weights = [ 0 ] * 9
				if expr.startswith('interpolate'):
					factorsStr = expr[
						expr.index('<') + 1 : expr.index('>')
						].split(',')
					pixelsStr = expr[
						expr.index('(') + 1 : expr.index(')')
						].split(',')
					assert len(factorsStr) == len(pixelsStr)
					for factorStr, pixelStr in izip(factorsStr, pixelsStr):
						factor = int(factorStr)
						pixelStr = pixelStr.strip()
						assert pixelStr[0] == 'c'
						pixel = int(pixelStr[1 : ]) - 1
						weights[pixel] = factor
				else:
					assert expr[0] == 'c'
					weights[int(expr[1 : ]) - 1] = 1
				pixelExpr[(case << 4) | subCase][subPixel] = weights

def pixelExpr8to9(pixelExpr2):
	pixelExpr3 = [ [ None ] * 9 for _ in range(1 << 12) ]
	for case in range(1 << 12):
		for subPixel in range(9):
			if subPixel < 4:
				pixelExpr3[case][subPixel] = pixelExpr2[case][subPixel]
			elif subPixel == 4:
				pixelExpr3[case][subPixel] = [0, 0, 0, 0, 1, 0, 0, 0, 0]
			else:
				pixelExpr3[case][subPixel] = pixelExpr2[case][subPixel - 1]
	return pixelExpr3

def sanityCheck(pixelExpr):
	'''Check various observed properties.
	'''
	subsets = [ (5, 4, 2, 1), (5, 6, 2, 3), (5, 4, 8, 7), (5, 6, 8, 9) ]

	for case, expr in enumerate(pixelExpr):
		# Weight factor of center pixel is never zero.
		# TODO: This is not the case for 3x, is that expected or a problem?
		#for corner in expr:
			#assert corner[5 - 1] != 0, corner

		# Sum of weight factors is always a power of two.
		for corner in expr:
			total = sum(corner)
			assert total in (1, 2, 4, 8, 16), total
			#
			count = reduce(lambda x, y: x + (y != 0), corner, 0)
			# at most 3 non-zero coef
			assert count <= 3, (case, corner)
			# and if there are 3 one of those must be c5
			assert (count < 3) or (corner[4] != 0)

		# Subpixel depends only on the center and three neighbours in the
		# direction of the subpixel itself.
		# TODO: This is not the case for 3x, is that expected or a problem?
		#for corner, subset in zip(expr, subsets):
			#for pixel in range(9):
				#if (pixel + 1) not in subset:
					#assert corner[pixel] == 0, corner

def genSwitch(pixelExpr):
	pixelExpr2 = pixelExpr8to9(pixelExpr)
	permutation = (2, 9, 7, 4, 3, 10, 11, 1, 8, 0, 6, 5)
	exprToCases = {}
	for case, expr in enumerate(pixelExpr2):
		exprToCases.setdefault(
			tuple(tuple(subExpr) for subExpr in expr),
			[]
			).append(permuteCase(permutation, case))
	yield 'switch (pattern) {\n'
	for cases, expr in sorted(
		( sorted(cases), expr )
		for expr, cases in exprToCases.iteritems()
		):
		for case in cases:
			yield 'case %d:\n' % case
		for subPixel, subExpr in enumerate(expr):
			if subPixel == 4:
				assert printSubExpr(subExpr) == 'c5'
			else:
				yield '\tpixel%d = %s;\n' % (
					subPixel + 1, printSubExpr(subExpr)
					)
		yield '\tbreak;\n'
	yield 'default:\n'
	yield '\tUNREACHABLE;\n'
	yield '\tpixel1 = pixel2 = pixel3 = pixel4 =\n'
	yield '\tpixel6 = pixel7 = pixel8 = pixel9 = 0; // avoid warning\n'
	yield '}\n'
	yield 'pixel5 = c5;\n'

tablePermutation = (5, 0, 4, 6, 3, 10, 11, 2, 1, 9, 8, 7)

def genHQLiteOffsetsTable(pixelExpr):
	pixelExpr2 = permuteCases(tablePermutation, pixelExpr)
	pixelExpr3 = pixelExpr8to9(pixelExpr2)

	offset_x = ( 43,   0, -43,  43,   0, -43,  43,   0, -43)
	offset_y = ( 43,  43,  43,   0,   0,   0, -43, -43, -43)
	for expr in pixelExpr3:
		for subPixel, weights in enumerate(expr):
			neighbours = computeNeighbours(weights)
			assert neighbours[1] is None, neighbours
			neighbour = neighbours[0]

			x = 128 + offset_x[subPixel]
			y = 128 + offset_y[subPixel]
			factor = sum(weights)
			if neighbour == 3:
				x -= 128 * weights[3] / factor
			elif neighbour == 5:
				x += 128 * weights[5] / factor
			else:
				assert neighbour is None, neighbour
			assert 0 <= x < 256, x
			assert 0 <= y < 256, y
			yield x
			yield y

def formatOffsetsTable(pixelExpr):
	pixelExpr2 = permuteCases(tablePermutation, pixelExpr)
	pixelExpr3 = pixelExpr8to9(pixelExpr2)
	for case, expr in enumerate(pixelExpr3):
		yield '// %d\n' % case
		for weights in expr:
			for x, y in transformOffsets(weights):
				yield ' %3d, %3d,' % (x, y)
			yield '\n'

def formatWeightsTable(pixelExpr, cellFunc):
	pixelExpr2 = permuteCases(tablePermutation, pixelExpr)
	pixelExpr3 = pixelExpr8to9(pixelExpr2)
	for case, expr in enumerate(pixelExpr3):
		yield '// %d\n' % case
		for weights in expr:
			for weight in transformWeights(weights, cellFunc):
				yield ' %3d,' % weight
			yield '\n'

def genOffsetsTable(pixelExpr):
	pixelExpr2 = permuteCases(tablePermutation, pixelExpr)
	pixelExpr3 = pixelExpr8to9(pixelExpr2)
	return computeOffsets(pixelExpr3)

def genWeightsTable(pixelExpr):
	pixelExpr2 = permuteCases(tablePermutation, pixelExpr)
	pixelExpr3 = pixelExpr8to9(pixelExpr2)
	return computeWeights(pixelExpr3, computeWeightCells)

def makeLite(pixelExpr, preferC6subPixels):
	# TODO: Rewrite hqcommon.makeLite() so it doesn't change its input.
	liteExpr = deepcopy(pixelExpr)
	commonMakeLite(liteExpr, preferC6subPixels)
	return liteExpr

def makeNarrow(pixelExpr):
	centerOnly = [0, 0, 0, 0, 1, 0, 0, 0, 0]
	return [
		[	blendWeights(expr[0], expr[1], 2, 1),
			blendWeights(expr[2], expr[1], 2, 1),
			blendWeights(expr[3], centerOnly, 2, 1),
			blendWeights(expr[4], centerOnly, 2, 1),
			blendWeights(expr[5], expr[6], 2, 1),
			blendWeights(expr[7], expr[6], 2, 1),
			]
		for expr in pixelExpr
		]

if __name__ == '__main__':
	#for case in range(1 << 12):
	#	for pixel in range(8):
	#		print case, pixel, pixelExpr[case][pixel]

	#pixelExpr = makeNarrow(pixelExpr)

	pixelExpr = Parser().pixelExpr
	writeBinaryFile('HQ3xOffsets.dat', genOffsetsTable(pixelExpr))
	writeBinaryFile('HQ3xWeights.dat', genWeightsTable(pixelExpr))
	#printText(formatOffsetsTable(pixelExpr))
	#printText(formatWeightsTable(pixelExpr, computeWeightCells))

	#pixelExpr = Parser().pixelExpr
	#pixelExpr = makeLite(pixelExpr, ())
	#printText(formatWeightsTable(pixelExpr, computeLiteWeightCells))

	pixelExpr = Parser().pixelExpr
	pixelExpr = makeLite(pixelExpr, (2, 4, 7))
	writeBinaryFile('HQ3xLiteOffset.dat', genHQLiteOffsetsTable(pixelExpr))

	pixelExpr = Parser().pixelExpr
	writeTextFile('HQ3xScaler-1x1to3x3.nn', genSwitch(pixelExpr))
	pixelExpr = makeLite(pixelExpr, ())
	writeTextFile('HQ3xLiteScaler-1x1to3x3.nn', genSwitch(pixelExpr))
