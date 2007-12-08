# $Id$

include build/node-start.mk

SUBDIRS:=manual

# Not all of the docs are useful for end users, either because they are about
# the implementation, or because they are too rough/unfinished.
# Below is the list of docs that should be included in installation.
INSTALL_DOCS:= \
	release-notes.txt release-history.txt \
	exampleconfigs.xml

DIST:=$(INSTALL_DOCS) \
	msxinfo-article.html schema1.png schema2.png \
	screenshot.png openmsx.sgml

include build/node-end.mk
