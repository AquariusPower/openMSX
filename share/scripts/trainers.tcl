set_help_text trainer \
{game trainers version 1.7

please join us looking for cheats and submit them to the www.openmsx.org forum

credits:
  copyright 2005-2006 Albert Beevendorp all rights reserved
  copyright 2005-2006 Patrick van Arkel all rights reserved
  copyright 2006 Benoit Delvaux all rights reserved

the definition of cheating according to google:

- defeat someone in an expectation through trickery or deceit
- deceiver: someone who leads you to believe something that is not true
- a deception for profit to yourself

although the definition of cheating is mostly negative
this time i turned it into something fun.

openmsx is made to work together with ips patches. Albert has some ips
patches ready at http://ips.tni.nl/ for even more cheating ;)

please cheat responsible!
}

#helper functions
proc poke {addr val} 	{debug write memory $addr $val}
proc peek {addr}	{return [debug read memory $addr]}

#trainers

proc trainer_f1spirit {} {
	#player 1 always first place
	poke 0xe331 1
	#all combi's with konami carts (thanks bifi)
	poke 0xe1de 2
	#escon
	poke 0xe1fd 1
	#hyperoff
	poke 0xe1d6 1
	#maxpoint
	poke 0xe1df 1
	#player 1 feul (finally!)
	poke 0xe310 255
	#player 1 no damage (this is a bitmask)
	poke 0xe328 0
	#player 2 fuel
	poke 0xe3d0 255
	#player 2 damage
	poke 0xe3e8 0
	#player 2 position
	poke 0xe3f1 1
	after time 1 trainer_f1spirit
}
proc trainer_bubblebobble {} {
	#invincible player 1
	poke 0xdadd 200
	#invincible player 2
	poke 0xdb6b 100
	#poke 0xdb6a 200
	#super bobble
	poke 0xdae8 255
	#shoot bubbles
	poke 0xdae9 0
	#shoot fire
	#!poke 0xdae9 1
	#extend filled
	#!poke 0xdaf4 255
	after time 1 trainer_bubblebobble
}

proc trainer_thecastleexcelent {} {
	#blue keys
	poke 0xe337 5
	#red keys
	poke 0xe338 5
	#purple keys
	poke 0xe339 5
	#green keys
	poke 0xe33a 5
	#blue keys
	poke 0xe33b 5
	#yellow keys
	poke 0xe33c 5
	#have map
	poke 0xe321 9
	#have air
	poke 0xe344 255
	#invincible
	poke 0xe343 255
	#lives
	poke 0xe336 255
	after time 1 trainer_thecastleexcelent
}

proc trainer_thecastle {} {
	#blue keys
	poke 0xe337 5
	#red keys
	poke 0xe338 5
	#purple keys
	poke 0xe339 5
	#green keys
	poke 0xe33a 5
	#blue keys
	poke 0xe33b 5
	#yellow keys
	poke 0xe33c 5
	#have map
	poke 0xe321 9
	#have air
	poke 0xe344 255
	#invincible
	poke 0xe343 255
	#lives
	poke 0xe336 255
	after time 1 trainer_thecastle
}

proc trainer_antartic_adventure {} {
	#time
	poke 0xe0e3 0x11
	poke 0xe0e4 0x1
	#short runs
	poke 0xe0e6 1
	#difficulty level
	#poke 0xe0e0 1
	after time 60 trainer_antartic_adventure
}

proc trainer_athleticland {} {
	#lives
	poke 0xe050 99
	#x-pos cabbage
	poke 0xe0e9 1
	#y-pos cabbage
	poke 0xe0e8 0
	#bird x-pos
	poke 0xe101 0	
	#bird y-pos
	poke 0xe100 0
	#stone x-pos
	poke 0xe109 0	
	#stone y-pos
	poke 0xe10a 0
	after time 0.1 trainer_athleticland
}


proc trainer_cabbagepatchkids {} {
	#lives
	poke 0xe050 99
	#x-pos cabbage
	poke 0xe0e9 1
	#y-pos cabbage
	poke 0xe0e8 0
	#bird x-pos
	poke 0xe101 0	
	#bird y-pos
	poke 0xe100 0
	#stone x-pos
	poke 0xe109 0	
	#stone y-pos
	poke 0xe10a 0
	after time 0.1 trainer_cabbagepatchkids
}

proc trainer_circuscharlie {} {
	#lives
	poke 0xe050 99
	after time 2 trainer_circuscharlie
}

proc trainer_comicbakkery {} {
	#lives
	poke 0xe050 99
	#beams
	poke 0xe120 99
	#items done
	poke 0xe060 16
	#all machines active
	poke 0xe057 112
	#sleepy beavers
	poke 0xe111 255
	poke 0xe113 255
	poke 0xe115 255
	after time 2 trainer_comicbakkery
}

proc trainer_frogger {} {
	#lives
	poke 0xe002 99
	#time
	poke 0xe052 16
	after time 2 trainer_frogger
}

proc trainer_pippols {} {
	#lives
	poke 0xe050 0x99
	#invincible
	poke 0xe11b 255
	#running shoes
	poke 0xe1a8 1
	#jump shoes
	poke 0xe1a9 1
	after time 2 trainer_pippols
}

proc trainer_kingsvalley1 {} {
	#lives
	poke 0xe050 99
	#door is always open
	poke 0xe1f2 1
	poke 0xe1f3 1
	after time 2 trainer_kingsvalley1
}

proc trainer_konamisboxing {} {
	#lives
	poke 0xe218 0
	#enemy live (KO after one hit)
	poke 0xe21a 9
	after time 2 trainer_konamisboxing
}

proc trainer_hyperrally {} {
	#always first place
	poke 0xe05c 1
	poke 0xe05b 0
	#fuel always full
	poke 0xe065 255
	after time 2 trainer_hyperrally
}

proc trainer_magicaltree {} {
	#99 lives
	poke 0xe050 99
	after time 2 trainer_magicaltree
}

proc trainer_mopiranger {} {
	#99 lives
	poke 0xe050 99
	#y-position grey enemy
	poke 0xe160 7
	#y-position red enemy
	poke 0xe190 7
	#y-position yellow enemy
	poke 0xe1c0 7
	#y-position blue enemy
	poke 0xe1f0 7
	#y-position Big Razzon
	poke 0xe220 7
	after time 1 trainer_mopiranger
}

proc trainer_qbert {} {
	#level (0-4)
	#poke 0xe103 0
	#lives
	poke 0xe110 0x99
	#stage (1-9;16-25;32-41;48-57;64-73;80)
	#poke 0xe111 1
	#no enemies
	poke 0xe321 255
	#always protected
	poke 0xe345 255
	#infinite time
	poke 0xec51 0x99
	after time 2 trainer_qbert
}

proc trainer_roadfighter {} {
	#99 lives
	poke 0xe083 220
	#y-position car 1
	poke 0xe0eb 191
	#x-position car 1
	poke 0xe0ed 0
	#y-position car 2
	poke 0xe0fb 191
	poke 0xe0fd 0
	after time 0.25 trainer_roadfighter
}

proc trainer_skyjaguar {} {
	#99 lives
	poke 0xe050 0x99
	after time 2 trainer_skyjaguar
}

proc trainer_supercobra {} {
	#99 lives
	poke 0xe050 0x99
	#fuel
	poke 0xe51c 128
	after time 2 trainer_supercobra
}

proc trainer_timepilot {} {
	#99 lives
	poke 0xe003 0x99
	after time 2 trainer_timepilot
}

proc trainer_twinbee {} {
	#lives player 1
	poke 0xe070 0x99
	#lives player 2
	poke 0xe073 0x99
	#speed player 1
	poke 0xe081 3
	#speed player 2
	poke 0xe082 3
	#shield and more player 1 (0-15)
	# 0 = single shoot
	# 1 or 9 = double shoot
	# 2 = options + single shoot
	# 3 or 11 = options + double shoot
	# 4 = shield + single shoot
	# 5 or 13 = shield + double shoot
	# 6 = options + shield + single shoot
	# 7 or 15 = options + shield + double shoot
	# 8 = spread shoot
	# 10 = options + spread shoot
	# 12 = shield + spread shoot
	# 14 = options + shield + spread shoot
	# some differences in dual mode
	poke 0xe083 7
	#shield and more player 2 (0-15)
	poke 0xe084 7
	after time 1 trainer_twinbee
}

proc trainer_yiearkungfu1 {} {
	#powerbar
	poke 0xe116 32
	#kill enemy with one hit
	poke 0xe117 0
	after time 5 trainer_yiearkungfu1
}

proc trainer_dota {} {
	#full energy
	poke 0xe49c 32
	poke 0xe49d 32
	#full ammo
	poke 0xe49e 10
	poke 0xe49f 10
	#full power schield
	poke 0xe504 10
	#have wings
	poke 0xe475 3
	#have always 9 keys
	poke 0xe470 9
	after time 2 trainer_dota
}

proc trainer_gradius1 {} {
	#lives
	poke 0xe060 0x99
	#stage (1-8=normal;9-12=bonus)
	#poke 0xe061 1
	#speed set to 4
	poke 0xe10b 4
	#missile
	poke 0xe132 1
	#double
	poke 0xe133 1
	#laser
	poke 0xe134 1
	#option
	poke 0xe135 1
	poke 0xe20b 2
	#shield
	poke 0xe136 2
	poke 0xe201 10
	#shield on (1=off)
	poke 0xe200 3
	#always hyper
	poke 0xe202 8
	#deactivate normal shot
	poke 0xe20c 0
	#enable double
	poke 0xe20d 2
	#enable laser
	poke 0xe20e 2
	#enable missile
	poke 0xe20f 2
	#enable option 1
	poke 0xe220 1
	#enable option 2
	poke 0xe240 1
	#simulate twinbee in slot 2
	#poke 0xf0f4 1
	#x-pos enemy
	#poke 0xe306 0
	#x-pos enemy
	#poke 0xe326 0
	#x-pos enemy
	#poke 0xe346 0
	#x-pos enemy
	#poke 0xe366 0
	#x-pos enemy
	#poke 0xe386 0
	#x-pos enemy
	#poke 0xe3a6 0
	#x-pos enemy
	#poke 0xe3c6 0
	#x-pos enemy
	#poke 0xe3e6 0
	#x-pos enemy
	#poke 0xe406 0
	#x-pos enemy
	#poke 0xe426 0
	#x-pos enemy
	#poke 0xe446 0

	after time 0.5 trainer_gradius1
}

proc trainer_gradius1scc {} {
	#lives
	poke 0xc060 0x99
	#stage (1-8=normal;9-12=bonus)
	#poke 0xc061 1
	#speed set to 4
	poke 0xc10b 4
	#missile
	poke 0xc132 1
	#double
	poke 0xc133 1
	#laser
	poke 0xc134 1
	#option
	poke 0xc135 1
	poke 0xc20b 2
	#shield
	poke 0xc136 2
	poke 0xc201 10
	#shield on (1=off)
	poke 0xc200 3
	#always hyper
	poke 0xc202 8
	#deactivate normal shot
	poke 0xc20c 0
	#enable double
	poke 0xc20d 2
	#enable laser
	poke 0xc20e 2
	#enable missile
	poke 0xc20f 2
	#enable option 1
	poke 0xc220 1
	#enable option 2
	poke 0xc240 1
	after time 2 trainer_gradius1scc
}

proc trainer_gradius2 {} {
	#lives
	poke 0xe200 0x99
	#stage (1-11;0)
	#1-7=normal 8=core
	#9-11=bonus 0=final
	#poke 0xe201 1
	#nice colors
	poke 0xe283 14
	poke 0xe408 15
	#shield 0=off 2=on
	poke 0xe400 2
	#speed set to 5
	poke 0xe402 5
	#options
	poke 0xe40b 2
	poke 0xe410 1
	poke 0xe420 1
	#deactivate normal shot
	poke 0xe430 0
	#double
	#poke 0xe431 2
	#laser (1-3;5-6)
	#1=normal 2=double 3=extended
	#5=reflex ring 6=fire blaster
	poke 0xe432 3
	#missile (1-3)
	#1=normal 2=double 3=napalm
	poke 0xe433 3
	#up laser
	poke 0xe434 2
	#down laser
	#poke 0xe435 2
	#back beam
	#poke 0xe436 2
	#special item (3-5;7)
	#3 = option ring
	#4 = rotary drill
	#5 = enemy slow
	#7 = vector laser
	poke 0xe439 3
	#fake metalion mode
	#poke 0xe446 1
	#simulate cartridge in slot 2
	# 1 = Q-Bert
	# 4 = Penguin Adventure
	# 8 = The Maze Of Galious
	# 255 = all the 3 cartridges
	#poke 0xf0f5 255
	#xpos enemy
	#poke 0xe606 0
	#xpos enemy
	#poke 0xe646 0
	#xpos enemy
	#poke 0xe686 0
	#xpos enemy
	#poke 0xe6c6 0
	#xpos enemy
	#poke 0xe706 0
	#xpos enemy
	#poke 0xe746 0
	#xpos enemy
	#poke 0xe786 0
	#xpos enemy
	#poke 0xe7c6 0
	after time 0.5 trainer_gradius2
}

proc trainer_gradius2beta {} {
	#lives
	poke 0xe200 0x99
	#stage (1-9;0)
	#1-8=normal 9=core
	#0=final
	#poke 0xe201 1
	#nice colors
	poke 0xe283 14
	poke 0xe408 15
	#shield 0=off 2=on
	poke 0xe400 2
	#speed set to 5
	poke 0xe402 5
	#options
	poke 0xe40b 2
	poke 0xe410 1
	poke 0xe420 1
	#deactivate normal shot
	poke 0xe430 0
	#double
	#poke 0xe431 2
	#laser (1-3;5-6)
	#1=normal 2=double 3=extended
	#5=reflex ring 6=fire blaster
	poke 0xe432 3
	#missile (1-3)
	#1=normal 2=double 3=napalm
	poke 0xe433 3
	#up laser
	poke 0xe434 2
	#down laser
	#poke 0xe435 2
	#back beam
	#poke 0xe436 2
	#special item (3-5;7)
	#3 = option ring
	#4 = rotary drill
	#5 = enemy slow
	#7 = vector laser
	poke 0xe439 3
	after time 2 trainer_gradius2beta
}	

proc trainer_golvellius1 {} {
	#max health and full bar
	poke 0xe022 240
	poke 0xe03d 240
	#all items (doesn't make much sense... non logical stuff)
	poke 0xe01d 7
	poke 0xe01f 3
	poke 0xe020 6
	poke 0xe021 2
	poke 0xe03c 1
	poke 0xe03f 5
	#leafs
	poke 0xe0a4 3
	#max gold
	poke 0xe050 255
	poke 0xe051 255
	#do not get paralized when hit by an enemy (should be done every frame)
	poke 0xd01a 0
	#get all christals (bitmask) (new)
	poke 0xe05f 255
	after time 1 trainer_golvellius1
}

proc trainer_goonies {} {
	#vitality
	poke 0xe064 80
	#experience
	poke 0xe065 80
	#stage (1-5)
	#poke 0xe06c 1
	#always have key
	poke 0xe121 1
	#open door to next stage
	poke 0xe130 7
	#protected from most enemies
	poke 0xe176 255
	#protected from water, fire, bats and shoots
	poke 0xe177 255
	#protected from falling stones
	poke 0xe178 1
	#extra vitality (1=experience)
	poke 0xe179 4
	#show hidden items
	poke 0xe2ed 2
	#
	#specific displayed items - 3 groups
	#for each item : an activation address and a display address
	#for 2 or more items in the same group : the value for the activation
	#must be the sum of the values required to activate each item -
	#you need also to change the value for the display address,
	#only the values 1-12 will allow the display at the bottom of the screen
	#!
	#items - 1st group
	#!
	#shoes
	#poke 0xe176 1
	#poke 0xe180 1
	#yellow book
	#poke 0xe176 2
	#poke 0xe181 1
	#white book
	#poke 0xe176 4
	#poke 0xe182 1
	#dark blue book
	#poke 0xe176 8
	#poke 0xe183 1
	#purple book
	#poke 0xe176 16
	#poke 0xe184 1
	#green book
	#poke 0xe176 32
	#poke 0xe185 1
	#red book
	#poke 0xe176 64
	#poke 0xe186 1
	#pale blue book
	#poke 0xe176 128
	#poke 0xe187 1
	#!
	#items - 2d group
	#!
	#blue raincoat
	#poke 0xe177 1
	#poke 0xe188 1
	#yellow thermic coat
	#poke 0xe177 2
	#poke 0xe189 1
	#grey raincoat
	#poke 0xe177 4
	#poke 0xe18a 1
	#green thermic coat
	#poke 0xe177 8
	#poke 0xe18b 1
	#yellow shield
	#poke 0xe177 16
	#poke 0xe18c 1
	#purple shield
	#poke 0xe177 32
	#poke 0xe18d 1
	#white shield
	#poke 0xe177 64
	#poke 0xe18e 1
	#blue helmet
	#poke 0xe177 128
	#poke 0xe18f 1
	#!
	#items - 3rd group
	#!
	#yellow helmet
	#poke 0xe178 1
	#poke 0xe190 1
	#hammer
	#poke 0xe178 2
	#poke 0xe191 1
	#lamp
	#poke 0xe178 4
	#poke 0xe192 1
	#false blue book
	#poke 0xe178 8
	#poke 0xe193 1
	#false purple book
	#poke 0xe178 16
	#poke 0xe194 1
	#false red book
	#poke 0xe178 32
	#poke 0xe195 1
	#clock
	#poke 0xe178 64
	#poke 0xe196 1
	after time 1 trainer_goonies
}

proc trainer_thexder1 {} {
	#energy
	poke 0xf2d4 255
	poke 0xf2d6 255
	#disable killer missles 255 = true / 0 = false
	poke 0xf2ec 255
	after time 2 trainer_thexder1
}

proc trainer_thexder2 {} {
	#do not loose power while shooting
	poke 0x12d5 255
	#shield power does not decline
	poke 0x12f8 255
	#missles
	poke 0x12e6 99
	#dart missles
	poke 0x134e 99
	#bomb (napalm)
	poke 0x134f 99
	#flashers
	poke 0x1350 99
	#max energy 500
	poke 0x12d8 250
	after time 2 trainer_thexder2
}

proc trainer_craze {} {
	#life
	poke 0xc054 20
	#ammo
	poke 0xc059 80
	#something
	poke 0xc05a 6
	#max shot
	poke 0xc064 255
	#max front shield
	poke 0xc067 255
	#max back shield
	poke 0xc069 255
	#max back shield
	poke 0xc069 255
	#side shield
	poke 0xc06a 255
	#spikes
	poke 0xc06a 255
	#trusters
	poke 0xc06c 255
	#wings
	poke 0xc06d 255
	after time 2 trainer_craze
}

proc trainer_zombie_hunter {} {
	#exp
	poke 0xc7e6 255
	poke 0xc7e7 255
	#life bar
	poke 0xc7ea 255
	poke 0xc7eb 255
	#max level
	poke 0xc7ee 31
	after time 1 trainer_zombie_hunter
}

proc trainer_xevious {} {
	#have all weapons and shield
	poke 0xc005 255
	#lives
	poke 0xc502 99
	after time 2 trainer_xevious
}

proc trainer_parodius {} {
	#lives
	poke 0xe240 0x99
	#stage (1-6=normal;7-9=bonus)
	#poke 0xe241 1
	#more bells
	poke 0xe251 255
	poke 0xe253 255
	poke 0xe254 255	
	poke 0xe256 255
	#full power
	poke 0xe268 4
	#speed set to 4
	poke 0xe335 4
	#shield 0=off 2=on
	poke 0xe400 2
	#shield
	poke 0xe402 2
	poke 0xe40a 15
	poke 0xeb07 15
	#option
	poke 0xe40b 2
	#enable option 1
	poke 0xe410 1
	#enable option 2
	poke 0xe420 1
	#disable normal shoot
	poke 0xe430 0
	#enable double
	#poke 0xe431 2
	#enable laser
	poke 0xe432 2
	#enable missile	
	poke 0xe433 2
	after time 1 trainer_parodius
}

proc trainer_salamander {} {
	#who needs a shield anyway
	poke 0xe202 0
	#lives player 1
	poke 0xe300 0x99
	#stage (1-7)
	# 1-6 = normal stage
	# 7 = special stage (requires Gradius 2 in slot 2)
	#poke 0xe301 1
	#scroll stop (only for part of stage 1)
	#poke 0xe309 1
	#all special weapons player 1
	poke 0xe310 7
	#extra level player 1 after one energy pod
	poke 0xe31b 0x14
	#option hold player 1
	#poke 0xe334 1
	#ship speed player 1 (1-7)
	poke 0xe337 5
	#laser shoot player 1 (1-7)
	# 1 = normal shoot
	# 2 = ripple laser
	# 3 = laser
	# 4 = screw laser
	# 5 = meteor laser
	# 6 = double laser
	# 7 = triple laser
	poke 0xe340 7
	#double way missile player 1 (1-4)
	# 1=normal 2=hawkwind 3=homing 4=arming ball
	poke 0xe341 3
	#trampling missile player 1
	#poke 0xe342 1
	#lives player 2
	poke 0xe380 0x99
	#all special weapons player 2
	poke 0xe390 7
	#extra level player 2 after one energy pod
	poke 0xe39b 0x14
	#option hold player 2
	#poke 0xe3b4 1
	#ship speed player 2 (1-7)
	poke 0xe3b7 5
	#laser shoot player 2 (1-7)
	#see explanations player 1
	poke 0xe3c0 7
	#double way missile player 2 (1-4)
	# 1=normal 2=hawkwind 3=homing 4=arming ball
	poke 0xe3c1 3
	#trampling missile player 2
	#poke 0xe3c2 1
	#option 1
	poke 0xe400 1
	#option 1 player 2 (0=player 1)
	#poke 0xe401 1
	#option 1 in position 1
	poke 0xe402 1
	#option 2 active
	poke 0xe410 1
	#option 2 player 2 (0=player 1)
	#poke 0xe411 1
	#option 2 in position 2
	poke 0xe412 2
	#option 3 active
	poke 0xe420 1
	#option 3 player 2 (0=player 1)
	#poke 0xe421 1
	#option 3 in position 3
	poke 0xe422 3
	#option 4 active (not in dual mode)
	poke 0xe430 1
	#option 4 in position 4
	poke 0xe432 4
	#spark light (only for planet Lavinia)
	#poke 0xe630 3
	after time 5 trainer_salamander
}
proc trainer_jackiechprotector {} {
	#life bar
	poke 0xe024 5
	#lives
	poke 0xe016 9
	after time 10 trainer_jackiechprotector
}

proc trainer_zanac {} {
	#invincible
	poke 0xe305 128
	poke 0xe31b 255
	#super shot
	poke 0xe10f 48
	#lives
	poke 0xe10a 99
	after time 1 trainer_zanac
}

proc trainer_zanac-ex {} {
	#lives
	poke 0xc012 100
	#primary weapon max level
	poke 0xc013 4
	poke 0xc016 4
	poke 0xc03f 3
	poke 0xc040 3
	poke 0xc104 2
	poke 0xc161 2
	#secondary weapon max level
	poke 0xc041 2
	#invincible
	poke 0xc405 128
	#set timer to max
	poke 0xc416 255
	after time 1 trainer_zanac-ex
}

proc trainer_mrghost {} {
	#life and attacks
	poke 0xc28e 255
	poke 0xc3a6 32
	poke 0xc01d 255
	poke 0xc01e 255
	#invincible
	poke 0xc291 255
	after time 10 trainer_mrghost
}

proc trainer_dragonbuster {} {
	#life and attacks
	poke 0xc312 0x99
	poke 0xc313 0x5
	#!exp
	#!poke 0xc2e2 0x99
	#!poke 0xc2e3 0x99
	#!poke 0xc2e4 0x99
	#!poke 0xc2fd 0x99
	#!poke 0xc2fe 0x99
	#!poke 0xc2ff 0x99

	after time 5 trainer_dragonbuster
}

proc trainer_feedback {} {
	#missles
	poke 0xd214 99
	#life
	poke 0xd213 16
	#speed
	poke 0xd212 10
	#invincible
	poke 0xd21b 255
	#red missle
	poke 0xd17a 255
	poke 0xd17e 2
	#have one red missle ready :)
	poke 0xd21a 1
	#make sure those stay on the screen
	poke 0xd10a 255
	poke 0xd11a 255
	poke 0xd12a 255
	poke 0xd13a 255
	poke 0xd14a 255
	poke 0xd15a 255
	poke 0xd16a 255
	after time 2 trainer_feedback
}

proc trainer_herzog {} {
	#own damage
	poke 0xd033 0
	#base damage
	poke 0xd034 0
	#max money
	poke 0xd035 255
	poke 0xd036 255
	#lives
	poke 0xd040 99
	#blow up player 2 base
	#poke 0xd087 255
	#wait off cheat (ctrl-esc-f5)
	poke 0xd1b3 255
	after time 2 trainer_herzog
}

proc trainer_xak1 {} {
	#exp
	poke 0x1c60 255
	poke 0x1c61 255
	#gold
	poke 0x1c62 255
	poke 0x1c63 255
	#life
	poke 0x2377 255
	#super latok mode
	poke 0x1fd4 1
	poke 0x2473 0x44

	#enchanted sword 1
	poke 0x1c13 99
	#enchanted sword 2
	poke 0x1c14 99
	#enchanted sword 3
	poke 0x1c15 99
	#enchanted sword 4
	poke 0x1c16 99
	#enchanted sword 5
	poke 0x1c17 99
	#enchanted sword 6
	poke 0x1c18 99

	#enchanted armor 1
	poke 0x1c19 99
	#enchanted armor 2
	poke 0x1c1a 99
	#enchanted armor 3
	poke 0x1c1b 99
	#enchanted armor 4
	poke 0x1c1c 99
	#enchanted armor 5
	poke 0x1c1d 99
	#enchanted armor 6
	poke 0x1c1e 99

	#enchanted armor 1
	poke 0x1c1f 99
	#enchanted armor 2
	poke 0x1c20 99
	#enchanted armor 3
	poke 0x1c21 99
	#enchanted armor 4
	poke 0x1c22 99
	#enchanted armor 5
	poke 0x1c23 99
	#enchanted armor 6
	poke 0x1c24 99

	#bread
	poke 0x1c25 99
	#meat
	poke 0x1c26 99
	#glasses
	poke 0x1c27 99
	#arm protector
	poke 0x1c28 99
	#blue ring
	poke 0x1c29 99
	#green (evil?) ring
	poke 0x1c2a 99
	#potion
	poke 0x1c2b 99
	#wheel
	poke 0x1c2c 99
	#purple cape
	poke 0x1c2d 99
	#feather/grass (?)
	poke 0x1c2e 99

	#light green scroll
	poke 0x1c2f 99
	#blue scroll
	poke 0x1c30 99
	#red scroll
	poke 0x1c31 99
	#green scroll
	poke 0x1c32 99
	#purple scroll
	poke 0x1c33 99

	#blue ball (no fun intended)
	poke 0x1c34 99
	#green ball
	poke 0x1c35 99
	#red ball
	poke 0x1c36 99

	#red cape
	poke 0x1c37 99
	#paint bucket (?)
	poke 0x1c38 99
	#paint boat (?)
	poke 0x1c39 99
	#water bottle (?)
	poke 0x1c3a 99
	#yellow key (?)
	poke 0x1c3b 99
	#blue key (?)
	poke 0x1c3c 99
	#treasure box
	poke 0x1c3d 99
	#bunny
	poke 0x1c3e 99
	#bunny
	poke 0x1c3e 99
	#necklace with stone
	poke 0x1c3f 99
	#butterfly broche
	poke 0x1c40 99
	#purple bottle
	poke 0x1c41 99
	#thee pot
	poke 0x1c42 99
	#red key
	poke 0x1c43 99
	#purple key
	poke 0x1c44 99					
	after time 2 trainer_xak1
}

proc trainer_sd-snatcher {} {
	#max out all stats
	poke 0xce82 255
	poke 0xce83 255

	poke 0xce85 255
	poke 0xce86 255

	poke 0xce88 255
	poke 0xce89 255

	poke 0xce8b 255
	poke 0xce8c 255

	#max life
	poke 0xce81 255

	#money
	poke 0xce8d 255
	poke 0xce8e 255

	#all locations accessible
	poke 0xcdc0 255
	poke 0xcdc1 1

	#junkers
	poke 0xc451 99
		
	#newtrits
	poke 0xc459 99
		
	#newtrits
	poke 0xc459 99
		
	#jyro
	poke 0xc461 99

	#bomb
	poke 0xc400 2
	poke 0xc401 255

	#dball
	poke 0xc408 2
	poke 0xc409 255

	#rancher
	poke 0xc410 2
	poke 0xc411 255

	#t blaster
	poke 0xc418 2
	poke 0xc419 255

	#g mine
	poke 0xc420 2
	poke 0xc421 255

	#comet
	poke 0xc428 2
	poke 0xc429 255

	#chaf
	poke 0xc430 2
	poke 0xc431 255

	#milkyway
	poke 0xc438 2
	poke 0xc439 255

	#c killer
	poke 0xc440 2
	poke 0xc441 255

	#flare
	poke 0xc448 2
	poke 0xc449 255

	#stringray
	poke 0xc3a8 2
	poke 0xc4a9 255
	#skill for stringay
	poke 0xc3ad 100
	#ammo for stingray
	poke 0xc3a9 231
	poke 0xc3aa 3

	#f. ball
	poke 0xc3b0 2
	poke 0xc4b1 255
	#skill for f. ball
	poke 0xc3b5 100
	#ammo for f. ball
	poke 0xc3b1 231
	poke 0xc3b2 3

	#k. sprint
	poke 0xc3b8 2
	poke 0xc4b9 255
	#skill for k. sprint
	poke 0xc3bd 100
	#ammo for k. sprint
	poke 0xc3b9 231
	poke 0xc3ba 3

	#storm
	poke 0xc3c0 2
	poke 0xc4c1 255
	#skill for storm
	poke 0xc3c5 100
	#ammo for storm
	poke 0xc3c1 231
	poke 0xc3c2 3

	#k. sprint
	poke 0xc3c8 2
	poke 0xc4c9 255
	#skill for k. sprint
	poke 0xc3cd 100
	#ammo for k. sprint
	poke 0xc3c9 231
	poke 0xc3ca 3

	#b. hawk
	poke 0xc3d0 2
	poke 0xc4d1 255
	#skill for b. hawk
	poke 0xc3d5 100
	#ammo for b. hawk
	poke 0xc3d1 231
	poke 0xc3d2 3

	#g. hound
	poke 0xc3d8 2
	poke 0xc4d9 255
	#skill for g. hound
	poke 0xc3dd 100
	#ammo for g.hound
	poke 0xc3d9 231
	poke 0xc3da 3

	#i. cepter
	poke 0xc3e0 2
	poke 0xc4e1 255
	#skill for i. cepter
	poke 0xc3e5 100
	#ammo for i. cepter
	poke 0xc3e1 231
	poke 0xc3e2 3

	#s. grade
	poke 0xc3e8 2
	poke 0xc4e9 255
	#skill for s. grade
	poke 0xc3ed 100
	#ammo for s. grade
	poke 0xc3e9 231
	poke 0xc3ea 3

	#n. point
	poke 0xc3f0 2
	poke 0xc4f1 255
	#skill for n. point
	poke 0xc3f5 100
	#ammo for n. point
	poke 0xc3f1 231
	poke 0xc3f2 3

	#big 9 matrix
	poke 0xc3f8 2
	poke 0xc4f9 255
	#skill for g. matric
	poke 0xc3fd 100
	#ammo for big 9 matrix
	poke 0xc3f9 231
	poke 0xc3fa 3

	#kill litle spiders in one blast
	poke 0xc820 0
	poke 0xc840 0

	#max rank
	poke 0xce80 64
	#max str level
	poke 0xce84 64
	#max def level
	poke 0xce87 64
	#max speed level
	poke 0xce8a 64

	#put weapons in slots (put the weapon value from 2 to 1 or else you will not be able to deselect it)
	#!poke 0xc150 1
	#!poke 0xc150 10
	#!poke 0xc150 11
}

proc trainer_sd-snatcher_walk_trough_walls {} {
	poke 0x92b6 0xc9
	after time 1 trainer_sd-snatcher_walk_trough_walls
}
proc trainer_undeadline {} {
	#lives
	poke 0xd2a9 2
	#power
	poke 0xd2a8 255
	#invincible to monsters
	poke 0xd2b7 255
	#weapons (0 knife/1 axe/2 fire/3 ice/4 tripple-knife/5 boomerang/6 vortex)
	poke 0xd2ab 6
	after time 2 trainer_undeadline
}

proc trainer_andorogynus  {} {
	#schield always on
	poke 0xeca1 255
	#lives
	poke 0xe01b 255
	#speed
	poke 0xec2b 16
	#shot strength
	poke 0xec3b 3
	#shot
	#! 1 = normal shot
	#! 2 = bigger shots
	#! 3 = pod with up/down shot
	#! 4 = laser
	#! 5 = big bouncing balls
	poke 0xec3a 5
	#backpack
	poke 0xec2c 2
	after time 1 trainer_andorogynus
}

proc trainer_aliens {} {
	#Ripley life
	poke 0x042a 255
	#Ripley ammo
	poke 0x042d 32
	after time 1 trainer_aliens
}

proc trainer_aliens2_msx1 {} {
	#invincible
	poke 0xe707 255
	#life bar
	poke 0xe247 16
	#m40 boms
	poke 0xe28f 250
	#twin pulse ammo
	poke 0xe28e 250
	after time 15 trainer_aliens2_msx1
}



proc trainer_galaga {} {
	#lives
	poke 57358 99
	after time 2 trainer_galaga
}

proc trainer_girlyblock {} {
	#player 1 life
	poke 0xe030 255
	#player 1 fuel
	poke 0xe031 255
	#player 1 level
	poke 0xe032 255
	#player 2 life
	poke 0xe0f0 0
	#player 2 fuel
	poke 0xe0f1 0
	#player 2 level
	poke 0xe0f2 0
	after time 2 trainer_girlyblock
}

proc trainer_fantasyzone1 {} {
	#money
	poke 0xe20d 0x99
	poke 0xe20c 0x99
	poke 0xe20b 0x99
	after time 2 trainer_fantasyzone1
}

proc trainer_fantasyzone2 {} {
	#money
	poke 0xe599 0x99
	poke 0xe59a 0x99
	poke 0xe59b 0x99
	#lives
	poke 0xe5ad 0x99
	after time 1 trainer_finasyzone2
}

proc trainer_richandmich {} {
	#shield
	poke 0xce13 255
	after time 10 trainer_richandmich
}

proc trainer_superpierot {} {
	#have ball
	poke 0xe7f0 1
	#lives
	poke 0xe046 255
	after time 1 trainer_superpierot
}

proc trainer_magicalwizzkid {} {
	#lives
	poke 0xc00c 255
	#diamond
	poke 0xc098 200
	#flask
	poke 0xc096 200
	#guardian angel
	poke 0xc095 200
	#fire
	poke 0xc094 200
	#explosion
	poke 0xc093 200
	#speed up potion
	poke 0xc092 200
	#time stopper
	poke 0xc091 200
	#power shot
	poke 0xc090 200
	#staff
	poke 0xc08f 200
	after time 2 trainer_magicalwizzkid
}

proc trainer_metalgear1 {} {
	#power bar
	poke 0xc131 48

	#handgun in slot 1
	poke 0xc500 1
	poke 0xc501 0x99
	poke 0xc502 0x9

	#smg in slot 2
	poke 0xc504 2
	poke 0xc505 0x99
	poke 0xc506 0x9

	#grenade launcher in slot 3
	poke 0xc508 3
	poke 0xc509 0x99
	poke 0xc50a 0x9

	#rocket launcher in slot 4
	poke 0xc50c 4
	poke 0xc50d 0x99
	poke 0xc50e 0x9

	#p-bomb in slot 5
	poke 0xc510 5
	poke 0xc511 0x99
	poke 0xc512 0x9

	#l-main in slot 6
	poke 0xc514 6
	poke 0xc515 0x99
	poke 0xc516 0x9

	#missle in slot 7
	poke 0xc518 7
	poke 0xc519 0x99
	poke 0xc51a 0x9

	#silencer in slot 4
	poke 0xc51c 8
	#enemies can't hurt you
	#!poke 0xc199 255

	#cart 1
	#poke 0xc5ad 1
	poke 0xc538 14
	poke 0xc539 49
	#cart 2
	#poke 0xc5ae 1
	poke 0xc53c 15
	poke 0xc53d 50
	#cart 3
	#poke 0xc5af 1
	poke 0xc540 16
	poke 0xc541 51
	#cart 4
	#poke 0xc5b0 1
	poke 0xc544 17
	poke 0xc545 52
	#cart 5
	#poke 0xc5b1 1
	poke 0xc548 18
	poke 0xc549 53
	#cart 6
	#poke 0xc5b2 1
	poke 0xc54c 19
	poke 0xc54d 54
	#cart 7
	#poke 0xc5b3 1
	poke 0xc550 20
	poke 0xc551 55
	#cart 8
	#poke 0xc5b4 1
	poke 0xc554 21
	poke 0xc555 56

	#ratio
	poke 0xc535 0x99
	poke 0xc534 22

	#armor
	poke 0xc558 1
	#bomb blast suit
	poke 0xc55c 2
	#flash light
	poke 0xc560 3
	#goggles
	poke 0xc564 4
	#gas mask
	poke 0xc568 5
	#mine detector
	poke 0xc56c 7
	#antenna
	poke 0xc570 8
	#parachute
	poke 0xc574 9
	#scope
	poke 0xc578 10
	#oxygen
	poke 0xc57c 11
	#compas
	poke 0xc580 12
	#antidote
	poke 0xc584 13
	#uniform
	poke 0xc588 24
	#box
	poke 0xc58c 25

	#put a nuclear warhead on your remote missles 8)
	if {[peek 0xc142] == 7} {
		 poke 0xc142 16
	 }
	#cart 1
	#if {[peek 0xc3d2] == 2}  {puts "cart 1"}
	#if {[peek 0xc3d2] == 66} {puts "cart 1"}
	#cart 2
	#if {[peek 0xc3d2] == 3}  {puts "cart 2"}
	#if {[peek 0xc3d2] == 67} {puts "cart 2"}

	#cart 3
	#if {[peek 0xc3d2] == 4}  {puts "cart 3"}
	#if {[peek 0xc3d2] == 68} {puts "cart 3"}
	#cart 4
	#if {[peek 0xc3d2] == 5}  {puts "cart 4"}
	#if {[peek 0xc3d2] == 69} {puts "cart 4"}
	#cart 5
	#if {[peek 0xc3d2] == 6}  {puts "cart 5"}
	#if {[peek 0xc3d2] == 76} {puts "cart 5"}
	#if {[peek 0xc3d2] == 70} {puts "cart 5"}
	#cart 6
	#if {[peek 0xc3d2] == 7}  {puts "cart 6"}
	#if {[peek 0xc3d2] == 47} {puts "cart 6"}
	#cart 7
	#if {[peek 0xc3d2] == 8}  {puts "cart 7"}
	#if {[peek 0xc3d2] == 72} {puts "cart 7"}

	#cart 1-8
	#!poke 0xc135 14
	#!poke 0xc135 15
	#!poke 0xc135 16
	#!poke 0xc135 17
	#!poke 0xc135 18
	#!poke 0xc135 19
	#!poke 0xc135 20
	#!poke 0xc135 21

	#active keycard
	#! poke 0xc135 x (13+x)

	#stop destruction timer
	#!poke 0xc13d 0x99
	#!poke 0xc13e 0x99

	#enemy 1 gone
	#poke 0xd005 0

	#enemy 2 gone
	#poke 0xd085 0

	#enemy 3 gone
	#poke 0xd105 0

	#enemy 4 gone
	#poke 0xd185 0
	
	#open all doors
	for {set i 0xc450} { $i < 0xc4fe } { incr i } {
		poke $i 0
	}

	for {set i 0xc490} { $i < 0xc4a9 } { incr i } {
		poke $i 0
	}
	after time 1 trainer_metalgear1
}

proc trainer_metalgear2 {} {
	#life bar
	poke 0xca53 32

	#get gun
	poke 0xd600 1
	poke 0xd601 0x99
	poke 0xd602 0x9

	#get sub machine gun
	poke 0xd604 1
	poke 0xd605 0x99
	poke 0xd606 0x9

	#get grendates
	poke 0xd608 1
	poke 0xd609 0x99
	poke 0xd60a 0x9

	#get surface to air missles
	poke 0xd610 1
	poke 0xd611 0x99
	poke 0xd612 0x9

	#get remote missles
	poke 0xd614 1
	poke 0xd615 0x99
	poke 0xd616 0x9

	#get c4 explosvies
	poke 0xd618 1
	poke 0xd619 0x99
	poke 0xd61a 0x9

	#get landmines
	poke 0xd61c 1
	poke 0xd61d 0x99
	poke 0xd61e 0x9

	#get camoflage
	poke 0xd620 1
	poke 0xd621 0x99
	poke 0xd622 0x9

	#get gas grenade
	poke 0xd624 1
	poke 0xd625 0x99
	poke 0xd626 0x9

	#get remote mice
	poke 0xd628 1
	poke 0xd629 0x99
	poke 0xd630 0x9

	#get zippo
	poke 0xd634 1
	poke 0xd635 1

	#get silencer
	poke 0xd62c 1
	poke 0xd62d 1

	#do not sink into the swamp
	poke 0xcb29 32

	#invisible until an alert is triggered
	poke 0xca3c 1

	#set avoiding time to 0
	poke 0xd42b 0

	#open path trough jungle
	poke 0xd430 2
after time 2 trainer_metalgear2
}

proc trainer_usas {} {
	#money
	poke 49753 0x99
	poke 49754 0x99
	#live for cles
	poke 0xc2d5 255
	#cles's stars for speed
	poke 0xc2d1 4
	#cles's stars for jumping
	poke 0xc2d2 2
	#cles's mood 0=happy, 1=good mood, 2=sad, 3=angry
	#!poke 0xc2b6 0
	#live for wit
	poke 0xc2b5 255
	#wit's stars for speed
	poke 0xc2b1 4
	#wit's stars for jumping
	poke 0xc2b2 2
	#wit's ulimited airwalk
	poke 0xc266 255
	#wit's mood 0=happy, 1=good mood, 2=sad, 3=angry
	#!poke 0xc2d6 0
	#all combi's with konami carts (thanks bifi)
	poke  0xc205 255
	#invinible player
	poke 0xc256 1
	#uncapture wit
	poke 0xc2b0 0
	#uncapture cless
	poke 0xc2d0 0
	#vitality rate 1 coin
	poke 0xc2b8 1
	poke 0xc2b9 0
	#vitality rate 1 coin
	poke 0xc2ba 1
	poke 0xc2bb 0
	#speed rate 1 coin
	poke 0xc2bc 1
	poke 0xc2bd 0
	#speed rate 1 coin
	poke 0xc2d8 1
	poke 0xc2d9 0
	#jump rate 1 coin
	poke 0xc2da 1
	poke 0xc2db 0
	#jump rate 1 coin
	poke 0xc2dc 1
	poke 0xc2dd 0
	#big door open
	#!poke 0xe328 1
	#!poke 0xe330 1
	#!poke 0xe338 1
	#!poke 0xe340 1

	#temple enemy dies after 1 hit/1st block (big enemy)
	poke 0xc412 1
	#2nd block 1st block (big enemy) gone after 1 hit
	poke 0xc492 1

	#wit can jump more than twice
	poke 0xc248 0
	poke 0xc265 0

	after time 1 trainer_usas
}

proc trainer_aleste1 {} {
	#lives
	poke 0xc010 98
	#invincible
	poke 0xc810 255
	#maxed up normal shot
	poke 0xc012 8
	#maxed up special shot
	poke 0xc019 3
	#scroll speed
	#poke 0xc4ad x
	#always keep weapon on 99
	poke 0xc01b 99
	after time 1 trainer_aleste1
}

proc trainer_aleste2 {} {
	#lives
	poke 0xc840 99
	#invincible
	poke 0xbc18 255
	#weapon has no 'time' limit
	poke 0xc84d 255
	#have weapon #
	poke 0xc84a 4
	#full power on weapons
	poke 0xc84f 5
	poke 0xc84e 50
	after time 1 trainer_aleste2
}

proc trainer_testament {} {
	#lives
	poke 0x59d7 144
	#handgranades
	poke 0x59de 32
	#map
	poke 0x59e3 1
	#strong bullets
	poke 0x59d9 255
	#shield
	poke 0x59dd 1
	after time 1 trainer_testament
}

proc trainer_ashiguine3 {} {
	#life
	poke 0xc0da 210
	#level up after killing one enemy
	poke 0xc0dd 206
	#money
	poke 0xc0df 255
	#loads of items
	poke 0xc598 1
	poke 0xc599 2
	poke 0xc59a 1
	poke 0xc59b 1
	poke 0xc59c 1
	poke 0xc59d 1
	poke 0xc59e 1
	poke 0xc59f 1
	poke 0xc5a0 1
	poke 0xc5a1 1
	after time 2 trainer_ashiguine3
}

proc trainer_ashiguine2 {} {
	#life
	poke 0xc016 255
	#keys
	poke 0xc022 99
	#enemy power (faster kills)
	poke 0xc052 1
	poke 0xc062 1
	poke 0xc072 1
	poke 0xc082 1
	poke 0xc092 1
	after time 2 trainer_ashiguine2
}

proc trainer_fparodic1 {} {
	#lives
	poke 0xe003 99
	#eggs
	poke 0xe025 255
	#invincible
	poke 0xe30e 255
	#full weapons
	poke 0xe050 4
	poke 0xe051 4
	poke 0xe052 4
	poke 0xe053 4
	poke 0xe054 4
	poke 0xe055 4
	poke 0xe056 4
	poke 0xe057 4
	poke 0xe058 4
	poke 0xe059 4
	after time 2 trainer_fparodic1
}

proc trainer_monmonmonster {} {
	#rocks
	poke 0xe038  99
	#lives
	poke 0xe02c 10
	#invincible
	poke 0xe31e 255
	#white power bolt shot
	poke 0xe02d 1
	#rotating shots
	poke 0xe02e 1
	after time 2 trainer_monmonmonster
}

proc trainer_maze_of_galious {} {
	#arrows
	poke 0xe046 0x99
	poke 0xe047 0x9
	#coin
	poke 0xe048 0x99
	poke 0xe049 0x9
	#keys
	poke 0xe04a 0x99
	poke 0xe04b 0x9
	#vit popolon
	poke 0xe056 255
	poke 0xe057 255
	#aphrodite
	poke 0xe053 255
	poke 0xe052 255
	#max exp
	poke 0xe051 1
	poke 0xe055 1
	#bible (ctrl) uses left
	poke 0xe531 255
	#zeus cheat
	poke 0xe027 1
	# active weapon (0 = nothing, 1 t/m 6 = arrow, cer.arrow, fire, rolling fire, mine, magnifying glass)
	#poke 0xe510 1
	#all combi's with konami carts (thanks bifi)
	poke 0xf0f8 255
	#arrows
	poke 0xe070 1
	#ceramic arrows
	poke 0xe071 1
	#rolling fire
	poke 0xe072 1
	#fire
	poke 0xe073 1
	#mine
	poke 0xe074 255
	#magnifying glass
	poke 0xe075 1
	#zeus  cheat(bifi)
	poke 0xe027 1
	#necklace
	poke 0xe07c 1
	#crown
	poke 0xe07d 1
	#helm
	poke 0xe07e 1
	#oar
	poke 0xe07f 1
	#boots
	poke 0xe080 1
	#decorative doll
	poke 0xe081 1
	#robe
	poke 0xe082 1
	#bell
	poke 0xe083 1
	#halo
	poke 0xe084 1
	#candle
	poke 0xe085 1
	#armor
	poke 0xe086 1
	#carpet
	poke 0xe087 1
	#helmet
	poke 0xe088 1
	#lamp
	poke 0xe089 1
	#vase
	poke 0xe08a 1
	#pendant
	poke 0xe08b 1
	#earrings
	poke 0xe08c 1
	#bracelet
	poke 0xe08d 1
	#ring
	poke 0xe08e 1
	#bible
	poke 0xe08f 1
	#harp
	poke 0xe090 1
	#triangle
	poke 0xe091 1
	#trumpet shell
	poke 0xe092 1
	#pitcher
	poke 0xe093 1
	#sabre
	poke 0xe094 1
	#dagger
	poke 0xe095 1
	#feather
	poke 0xe096 1
	#shield
	poke 0xe097 3
	#bread and water
	poke 0xe098 1
	#salt
	poke 0xe099 1
	#cross
	poke 0xe07a 1

	#use bible until kingdom come
	poke 0xe531 1
	#screen stays frozen for as long as you are in that screen
	poke 0xe0d6 64

	#world 10 location 0 = middle tower 1 = right tower 2 = left tower 3 = start
	poke 0xe06e	3
	poke 0xe06d	 0x01

	#world 1 items
	#poke 0xe063 0xf0
	#world 2 items
	#poke 0xe064 0xf0
	#world 3 items
	#poke 0xe065 0xf0
	#world 4 items
	#poke 0xe066 0xf0
	#world 5 items
	#poke 0xe067 0xf0
	#world 6 items
	#poke 0xe068 0xf0
	#world 7 items
	#poke 0xe069 0xf0
	#world 8 items
	#poke 0xe06a 0xf0
	#world 9 items
	#poke 0xe06b 0xf0
	#world 10 items
	#poke 0xe06c 0xe0

	#poke unvulnerable
	poke 0xe518 1
	#big enemy dies after 1 hit
	poke 0xe710 1
	#blink every second
	#!poke 0xec87 4
	after time 1 trainer_maze_of_galious
}

proc trainer_vampirekiller {} {
	#lives
	poke 0xc410 0x99
	#hearts
	poke 0xc417 0x99
	#power
	poke 0xc415 32
	#invisible to enemies
	#!poke 0xc43a 255
	#invincible
	poke 0xc42d 255
	#always have the white key
	poke 0xc701 255
	#always have small key
	poke 0xc700 1
	#always have map
	poke 0xc70f 3
	#weapon 0 regular whip 1 chain whip 2 knifes 3 axe 4 blue cross  5 holy water
	poke 0xc416 2
	#game master combo (thanks bifi)
	poke 0xe600 255
	#main enemy dies after 1 hit
	poke 0xc418 0
	#silver cross is always active (no irregular enemies)
	poke 0xc440 255
	#x-pos enemy 1
	#poke 0xc805 128
	#x-pos enemy 2
	#poke 0xc885 128
	after time 2 trainer_vampirekiller
}

proc trainer_superlaydock_mission_striker {} {
	#power player 1
	poke 0xe2f3 255
	#power player 2
	#!poke 0xe2fb 255
	#all weapons player 1
	poke 0xe480 255
	#all weapons player 2
	poke 0xe481 255
	#docking
	#!poke 0xe37c 200
	after time 2 trainer_superlaydock_mission_striker
}
proc trainer_superlaydock2  {} {
	#power
	poke 0x6817 255
	after time 2 trainer_superlaydock2
}

proc trainer_americantruck  {} {
	#disable collisions
	poke 0xf29a 255
	poke 0xf2a7 0
	after frame trainer_americantruck
}

proc trainer_guardic {} {
	#lives
	poke 0xe027 255
	#power
	poke 0xe019 255
	#have shield
	poke 0xe00c 1
	#speed
	poke 0xe00f 6
	#wave
	poke 0xe00a 4
	after time 60 trainer_guardic
}

proc trainer_laydock {} {
	#power player 1
	poke 0xa168 0x99
	poke 0xa169 0x99
	#power player 2
	poke 0xa179 0x99
	poke 0xa17a 0x99
	after time 2 trainer_laydock
}

proc trainer_spacemanbow {} {
	#stage (0-8)
	#poke 0xca10 0
	#invincible
	poke 0xca53 03
	poke 0xca54 03
	#option 1
	poke 0xcac0 2
	poke 0xcac1 3
	poke 0xcad8 253
	#option 2
	poke 0xcae0 2
	poke 0xcae1 3
	poke 0xcaf8 2
	#speed (1-4)
	poke 0xcb01 2
	#power bar
	poke 0xcb08 16
	#lives
	poke 0xcb0f 0x99
	#missile
	poke 0xcb48 128
	poke 0xcb49 3
	#way option 1 (6=back 7=up 8=front)
	poke 0xcb50 6
	#enable option 1
	poke 0xcb51 1
	#way option 2 (2=front 3=down 4=back)
	poke 0xcb58 4
	#enable option 2
	poke 0xcb59 1
	after time 1 trainer_spacemanbow
}

proc trainer_fantasmsoldier1 {} {
	#life
	poke 0xf064 255
	#max sword
	poke 0xf294 3
	after time 2 trainer_fantasmsoldier1
}

proc trainer_fantasmsoldier2 {} {
	#life
	poke 0xf937 255
	#invincible
	poke 0xf976 255
	#pearls
	poke 0xf969 99
	#shot strength
	poke 0xf977 4
	after time 2 trainer_fantasmsoldier2
}

proc trainer_dirdeaf {} {
	#life
	poke 0xb9ca 64
	#weapon 2
	poke 0xb8b0 1
	#weapon 3
	poke 0xb8b1 1
	#weapon 4
	poke 0xb8b2 1
	#weapon 5
	poke 0xb8b3 1
	#weapon 6
	poke 0xb8b4 1
	#weapon 57
	poke 0xb8b35 1
	#weapon 58
	poke 0xb8b6 1

	#life container 1
	poke 0xb8bb 255
	#life container 2
	poke 0xb8bc 255

	#defence 1
	poke 0xb8b7 1
	#defence 2
	poke 0xb8b8 1
	#defence 3
	poke 0xb8b9 1
	#defence 4
	poke 0xb8ba 1

	#card1
	poke 0xb8bf 1
	#card2
	poke 0xb8c0 1
	#card3
	poke 0xb8c1 1
	#card4
	poke 0xb8c2 1
	#card5
	poke 0xb8c3 1
	#card6
	poke 0xb8c4 1
	#card7
	poke 0xb8c5 1
	#card8
	poke 0xb8c6 1
	after time 2 trainer_dirdeaf
}

proc trainer_supercooks {} {
	#life
	poke 0xcfa1 0x00
	poke 0xcfa2 0x02
	#max hearts
	poke 0xcf9d 0x00
	poke 0xcf9e 0x02
	#dish
	poke 0xcfac 0x99
	poke 0xcfad 0x99
	after time 10 trainer_supercooks
}

proc trainer_golvellius2 {} {
	#life
	poke 0xcba1 0x00
	poke 0xcba2 0x02
	#max hearts
	poke 0xcb9d 0x00
	poke 0xcb9e 0x02
	#find
	poke 0xcbac 0x99
	poke 0xcbad 0x99

	#iron sword
	#!poke 0xcb01 1
	#bronze sword
	#!poke 0xcb02 1
	#gold sword
	poke 0xcb03 1
	#water boots
	poke 0xcb04 1
	#air boots
	poke 0xcb05 1
	#scepter
	poke 0xcb06 1
	#iron shield
	#!poke 0xcb07 1
	#bronze shield
	poke 0xcb08 1
	#gold ring
	poke 0xcb09 1
	#harp
	#!poke 0xcb0a 1
	#heart pendant
	#!poke 0xcb0b 1
	#candle
	#!poke 0xcb0c 1
	#mirror
	#!poke 0xcb0d 1
	#silver ring
	poke 0xcb0e 1
	#blue potion
	#!poke 0xcb0f 1
	#blue diamond
	#!poke 0xcb10 1
	#silver broche
	#!poke 0xcb11 1
	#fruit
	#!poke 0xcb12 1
	#fairy
	#!poke 0xcb13 1
	#necklace
	#!poke 0xcb14 1
	#golden lost ring
	#!poke 0xcb15 1
	#document to get first sword
	#!poke 0xcb16 1
	#key
	poke 0xcb17 1
	#herb
	#!poke 0xcb18 1
	after time 10 trainer_golvellius2
}

proc trainer_starquake {} {
	#lives
	poke 0x4061 64
	#life
	poke 0x4062 128
	#steps
	poke 0x4063 128
	#fire
	poke 0x4064 128
	after time 10 trainer_starquake
}

proc trainer_gryzor {} {
	#power bar
	poke 0xe2c9 32
	#invincible
	poke 0xe31e 200
	#normal gun
	#poke 0xe032 0
	#laser gun
	#poke 0xe032 1
	#rotating gun
	#poke 0xe032 2
	#machine gun
	#poke 0xe032 3
	#circling gun
	poke 0xe032 4
	#2 way gun
	#poke 0xe032 5
	#4 way gun
	#poke 0xe032 6
	#4 fragment gun
	#poke 0xe032 7
	#underground stages get easier
	poke 0xe50b 200
	#end bosses and underground stages get easier
	poke 0xe50b 200
	poke 0xe51b 200
	poke 0xe52b 200
	poke 0xe53b 200
	poke 0xe54b 200
	poke 0xe55b 200
	poke 0xe56b 200
	poke 0xe57b 200
	poke 0xe58b 200
after time 3 trainer_gryzor
}

proc trainer_rastansaga  {} {
	#life bar
	poke 0xd91c 160
	#get firesword
	poke 0xd919 3
	#weapon expiration timer
	poke 0xd91a 255
after time 2 trainer_rastansaga
}

proc trainer_outrun  {} {
	#time
	poke 0xc093 99
after time 2 trainer_outrun
}

proc trainer_ys1  {} {
	#life
	poke 0xcfc3 255
	#money
	poke 0xcfc9 255
	poke 0xcfca 255
	#exp
	poke 0xcfc7 255
	poke 0xcfc8 255
	#all swords
	poke 0xcfd9 31
	#all shields
	poke 0xcfdb 31
	#all armor
	poke 0xcfdd 31
	#all items
	poke 0xcfdf 31
	#all rings
	poke 0xcfe1 31
	#all books
	poke 0xcfe3 255
	#all special items
	poke 0xcfe4 255
	poke 0xcfe5 255
	after time 2 trainer_ys1
}

proc trainer_ys2 {} {
	#exp max
	poke 0x0102 255
	poke 0x0103 255
	#gold max
	poke 0x0196 255
	poke 0x0197 255
	#power meter
	poke 0x018c 255
	#magic meter
	poke 0x0104 255
	after time 2 trainer_ys2
}

proc trainer_penguinadventure  {} {
	#shoes
	poke 0xe160 1
	#proppelor
	poke 0xe161 1
	#gun
	poke 0xe162 1
	#lightning helmet
	poke 0xe163 3
	#helmet
	poke 0xe164 3
	#protective vest
	poke 0xe165 3
	#bell
	poke 0xe166 1
	#silver ring
	poke 0xe167 1
	#bracelet
	poke 0xe168 1
	#red pendant
	poke 0xe169 1
	#spectacles
	poke 0xe16a 2
	#torch
	poke 0xe16b 3
	#pass
	poke 0xe16c 1
	#blue boots
	poke 0xe16d 1
	#red shoes for extra grip
	poke 0xe16e 1
	#feather
	poke 0xe16f 1
	#secret items
	poke 0xe170 1
	poke 0xe171 1
	poke 0xe172 1
	#invincible
	poke 0xe1f1 1
	poke 0xe089 0x99
	#lives
	poke 0xe090 0x99
	#poke time never runs out
	poke 0xe08b 0x00
	poke 0xe08c 0x3
	#pause counter (good ending i have been told)
	poke 0xe0de 1
	#noriko cheat(thanks bifi)
	poke 0xf0f7 254
	#!slot machine
	#! 0= cherry
	#! 1= lemmon
	#! 2= grapes
	#! 3= konami logo
	#! 4= penguin
	#! 5= skull
	#!slot 1
	#!poke 0xe12d 0xa
	#!poke 0xe130 0xa
	#!slot 2
	#!poke 0xe12e 0xa
	#!poke 0xe131 0xa
	#!slot 3
	#!poke 0xe12f 0xa
	#!poke 0xe132 0xa

	#kill dragon with one shot
	poke 0xe53c 19

	#get 8 dancing penguins
	poke 0xe0dd 8

	after time 15 trainer_penguinadventure
}

proc trainer_stoneofwisdom {} {
	#life
	poke 0xe044 55
	#power
	poke 0xe042 55
	#intel
	poke 0xe040 55
	after time 2 trainer_stoneofwisdom
}
proc trainer_blowup {} {
	#'cosmic' cheat active f1 to place bombs f5 to blow yourself up
	poke 0x403b 1
	after time 2 trainer_blowup
}

proc trainer_arsenelupin3 {} {
	# life
	poke 0xe18e 40
	poke 0xe18f 40
	# bullet
	poke 0xe16f 0x99
	# missle and rings
	#poke 0xe1ca 1 missle has to turn to 0 use other cheat
	poke 0xe1cd 3
	poke 0xe269 14
	after time 2 trainer_arsenelupin3
}

proc trainer_arsenelupin2 {} {
	#power
	poke 0xc07f 200
	#invincible (makes game unplayable)
	#poke 0xc09b 255
after time 1 trainer_arsenelupin2
}

proc trainer_arsenelupin3_missle {} {
	poke 0xe1ca 1
}
proc trainer_superrunner {} {
	#invincible
	poke 0xccce 1
	poke 0xcccf 255
	#time
	poke 0xcf23 59
after time 5 trainer_superrunner
}

proc trainer_xyz {} {
	#power
	poke 0xe060 0x99
	#something else
	poke 0xe061 0x99
	#dunno
	poke 0xe062 0x99
after time 5 trainer_xyz
}

proc trainer_dragonslayer4_msx2 {} {
	#life
	poke 0xc067 109
	#magic
	poke 0xc068 109
	#money
	poke 0xc069 109
	#keys
	poke 0xc06a 109

	#wings
	poke 0xc06f 99
	#armor
	poke 0xc070 99
	#pick axe
	poke 0xc071 99
	#the glove
	poke 0xc072 99
	#speer hook
	poke 0xc073 99
	#spike shoes
	poke 0xc074 99
	#spring shoes
	poke 0xc075 99
	#master key
	poke 0xc076 99

	#helmet
	poke 0xc077 99
	#scepter
	poke 0xc078 99
	#dragon shield
	poke 0xc079 99
	#life potion
	poke 0xc07a 99
	#magic potion
	poke 0xc07b 99
	#red globe
	poke 0xc07c 99
	#crown
	poke 0xc07d 1
	#sword
	poke 0xc07e 99

	#invincible
	#!poke 0xc08b 255
	#jump high
	poke 0xc06b 40
	#strong weapons
	poke 0xc06c 255
	#walk trough air
	poke 0xc08c 2
	#kill enemies with body
	poke 0xc08d 2
	#shoot far
	poke 0xc06e 40
	#walk faster
	poke 0xc08e 255

	#put in slot 1 : pick axe
	#poke 0xc060 2
	#put in slot 2 : harpoon
	#poke 0xc061 4
	#put in slot 3 : crown
	#poke 0xc062 14
	#in game player (try different values)
	#!poke 0xc050 0

after time 1 trainer_dragonslayer4_msx2
}

proc trainer_dragonslayer4_msx1 {} {
	#life
	poke 0xe093 100
	#magic
	poke 0xe094 100
	#money
	poke 0xe095 100
	#keys
	poke 0xe096 100

	#wings
	poke 0xe09b 99
	#armor
	poke 0xe09c 99
	#pick axe
	poke 0xe09d 99
	#the glove
	poke 0xe09e 99
	#speer hook
	poke 0xe09f 99
	#spike shoes
	poke 0xe0a0 99
	#spring shoes
	poke 0xe0a1 99
	#master key
	poke 0xe0a2 99

	#helmet
	poke 0xe0a3 99
	#scepter
	poke 0xe0a4 99
	#dragon shield
	poke 0xe0a5 99
	#life potion
	poke 0xe0a6 99
	#magic potion
	poke 0xe0a7 99
	#red globe
	poke 0xe0a8 99
	#crown
	poke 0xe0a9 1
	#sword
	poke 0xe0aa 99

	#jump high
	poke 0xe097 40
	#strong weapons
	poke 0xe098 99

	after time 1 trainer_dragonslayer4_msx1
}

proc trainer_druid {} {
	poke 0xc06e 140
	#ammo 1
	poke 0xc024 99
	#ammo 2
	poke 0xc025 99
	#ammo 3
	poke 0xc026 99

	#key
	poke 0xc027 99
	#timer
	poke 0xc028 99
	#gollem
	poke 0xc029 99
	#death
	poke 0xc02a 99

	after time 2 trainer_druid
}
proc trainer_eggerland1 {} {
	#bullets
	poke 0xc811 0x99
	#lives
	poke 0xd0d0 0x99
	#door is always open
	poke 0xd1f2 0
	#blocks collected
	poke 0xd1f4 1
	#time in special stages
	poke 0xc81d 255
	#stage number
	#!poke c81b x
after time 1 trainer_eggerland1
}

proc trainer_eggerland2 {} {
	#shots always active
	poke 0xeb8a 97
	#lives
	poke 0xeb89 97
	#timer in special stages
	poke 0xec56 99
	#containers left
	poke 0xeb88 0
	#stay alive
	poke 0xeb94 0
	after time 1 trainer_eggerland2
}

proc trainer_wonderboy {} {
	#power
	poke 0xe0b3 32
	#lives
	poke 0xe0b2 10
	#invincible
	poke 0xe0a9 255
	after time 2 trainer_wonderboy
}

proc trainer_crossblaim {} {
	#money 999999
	poke 0xeebb 0x99
	poke 0xeebc 0x99
	poke 0xeebd 0x99

	#power
	poke 0xead4 255

	#engine 1
	poke 0xeeab 255
	#engine 2
	poke 0xeeac 255
	#engine 3
	poke 0xeead 255

	#power container 1
	poke 0xeeae 250
	#power container 2
	poke 0xeeaf 250

	#big laser gun
	poke 0xeea2 255
	#big laser gun 2
	poke 0xeea3 255

	#bullet for gun (#1)
	poke 0xeeb6 255
	#bullet for bazooka (#1)
	poke 0xeeb8 255
	#handgranades
	poke 0xeea6 255
	#darts
	poke 0xeea7 255

	#all keys
	poke 0xeeb0 1
	poke 0xeeb1 1
	poke 0xeeb2 1
	poke 0xeeb3 1
	poke 0xeeb4 1
}

proc trainer_knightmare {} {
	#lives
	poke 0xe060 0x99
	#invisible=2 redhot=3
	poke 0xe60c 2
	#timer
	poke 0xe60e 0x99
	#nuclear arrows ;)
	#poke 0xe609 13
	#enemy 1 y-pos
	poke 0xe103 210
	#enemy 2 y-pos
	poke 0xe113 210
	#enemy 3 y-pos
	poke 0xe123 210
	#enemy 4 y-pos
	poke 0xe143 210
	#enemy 5 y-pos
	poke 0xe163 210
	#end boss dies easily
	poke 0xe034 0
	after time 0.25 trainer_knightmare
}

proc trainer_quinpl {} {
	#timer
	poke 0xe231 0x99
	poke 0xe233 0x99
	#black pin
	poke 0xe020 0x9
	#white pin
	poke 0xe021 0x9
	#red pin
	poke 0xe022 0x9
	#blue pin
	poke 0xe023 0x9
	#duck
	poke 0xe024 0x9
	after time 1 trainer_quinpl
}

proc trainer_nyancleracing {} {
	#time
	poke 0xd213 255
	#candy
	poke 0xd215 255
	#damage
	poke 0xd217 0
	poke 0xd21e 0
	#invincible
	poke 0xd73e 255
	after time 1 trainer_nyancleracing
}

proc trainer_kingsvalley2 {} {
	#festival cheat
	poke 0xe255 1
	#try again  (thanks bifi)
	poke 0xe217 1
	#door always opens
	poke 0xe2f5 0
	#stage
	#poke 0xe242 1
	#lives
	poke 0xe240 0x99
	after time 2 trainer_kingsvalley2
}
proc trainer_malayanohibou {} {
	#life
	poke 0xccd3 15
	#money
	poke 0xccd4 255
	poke 0xccd5 255
	#have fires
	poke 0xcd74 99
	#have keys
	poke 0xcd75 99
	#have bombs
	poke 0xcd76 99
	#have potions
	poke 0xcd77 99
	#enable fire shooting
	poke 0xcd25 31
	#lives
	poke 0xccf3 255
	after time 2 trainer_malayanohibou
}

proc trainer_ninjakun {} {
	#lives
	poke 0xcb37 99
	#protection scrolls
	poke 0xcb2e 255
	#time
	poke 0xcb35 99
	after time 1 trainer_ninjakun
}

proc trainer_projecta2 {} {
	#power
	poke 0xc476 100
	after time 1 trainer_projecta2
}

proc trainer_returnofjelda {} {
	#power
	poke 0xc725 100
	#damage
	poke 0xc724 0
	after time 1 trainer_returnofjelda
}

proc trainer_scrambleformation {} {
	#help planes
	poke 0xc102 5
	#lives
	poke 0xc101 0x99
	after time 1 trainer_scrambleformation
}

proc trainer_chukataisen {} {
	#fire power
	poke 0xa67f 7
	#alternative firepower
	poke 0xa681 11
	#lives
	poke 0xa685 99
	#invincible
	poke 0xa683 1
	after time 1 trainer_chukataisen
}

proc trainer_goemon {} {
	#this cheat works for both players
	#lives
	poke 0xc260 0x99
	#power
	poke 0xc481 255
	#a lot of money
	poke 0xc265 255
	poke 0xc266 255
	#have money cases
	poke 0xc279 3
	#have candle
	poke 0xc278 1
	#have food
	poke 0xc277 5
	#have helmet
	poke 0xc276 5
	#have umbrella
	poke 0xc275 5
	#have 'shower gel' (?)
	poke 0xc274 5
	#have tent
	poke 0xc273 5
	#have helmet
	poke 0xc272 5
	#have catapult
	poke 0xc271 1
	#have shoes x3
	poke 0xc270 3
	#cart combi's (thanks bifi)
	poke 0xef00 255
	#invincible
	poke 0xc4a7 255
	after time 2 trainer_goemon
}

proc trainer_garryuuo {} {
	#invincible
	poke 0xe0b7 255
	#extra invincible
	poke 0xe06d 8
	#easy bosses (kill after one hit)
	poke 0xe1f1 1
	#freeze enemies
	poke 0xe06b 8
	after time 4 trainer_garryuuo
}

proc trainer_familyboxing {} {
	#power
	poke 0xc008 0
	poke 0xc010 0
	poke 0xc04e 28
	poke 0xc04f 28
	after time 1 trainer_familyboxing
}

proc trainer_kingkong2 {} {
	#life
	poke 0xc129 0x99
	#days passed
	poke 0xc128 0x00
	poke 0xc127 0x00
	#days exp
	poke 0xc12b 0x99
	poke 0xc12c 0x99
	#level
	poke 0xc12e 0x99
	#money
	poke 0xc135 0x99
	poke 0xc136 0x99

	#knife
	poke 0xc2a0 1
	poke 0xc340 255
	#club
	poke 0xc2a2 2
	poke 0xc341 255
	#stones
	poke 0xc2a4 8
	poke 0xc347 0x99
	#boomerang
	poke 0xc2a6 10
	poke 0xc349 0x99

	#vortex
	poke 0xc2c0 34
	poke 0xc2c1 255
	poke 0xc361 255

	after time 2 trainer_kingkong2
}

proc trainer_ikariwarriors {} {
	#rapid fire (set interval between shots)
	poke 0xc418 0

	#set kind of ammo for weapons 1 and 2
	# 0 = nothing
	# 1 = regular
	# 2 = 3 way shot (not deadly)
	# 3 = 7 way shot (not deadly)
	# 4 = red bullets
	# 5 = regular tank bullets
	# 6 = high explosive tank bullets
	# 7 = hand granates
	# 8 = high explosive hand granades

	#1st weapon player 1
	poke 0xc41d 6
	#2nd weapon player 1
	poke 0xc41e 8
	#lives player 1
	poke 0xc415 99

	#1st weapon player 2
	poke 0xc43d 6
	#2nd weapon player 2
	poke 0xc43e 8
	#lives player 2
	poke 0xc435 99

	#fun with spites
	#poke 0xc416 [sprite]
	after time 2 trainer_ikariwarriors
}

proc trainer_firebird {} {

	#ilovehinotori
	poke 0xc4e2 1
	#turbo
	poke 0xc850 3
	#autoshot
	poke 0xc85c 2
	poke 0xc4e1 2
	#99 lives
	poke 0xc160 0x99
	#200 money
	poke 0xc845 200

	#shoes
	poke 0xc850 3
	#bug (red beetle?)
	poke 0xc85c 9
	#compas
	poke 0xc884 1

	#packages
	poke 0xc870 9
	#scrolls
	poke 0xc874 9
	#leaflets (?)
	poke 0xc878 9

	#top 1st last stone
	poke 0xc88c 1
	#top 2nd last stone
	poke 0xc890 1
	#top 3th last stone
	poke 0xc894 1
	#top 4th last stone
	poke 0xc898 1
	#top 5th last stone
	poke 0xc89c 1
	#top last stone
	poke 0xc8a0 1

	#1st midle stone
	poke 0xc8bc 1
	#2nd midle stone
	poke 0xc8c0 1
	#3rd midle stone
	poke 0xc8c4 1
	#4rd midle stone
	poke 0xc8c8 1
	#5th midle stone
	poke 0xc8cc 1
	
	#lower first stone
	poke 0xc8a4 1
	#lower second stone
	poke 0xc8a8 1
	#lower third stone
	poke 0xc8ac 1
	#lower fouth stone
	poke 0xc8b0 1
	#lower fith stone
	poke 0xc8b4 1
	#lower sixth stone
	poke 0xc8b8 1

	#first main stone
	poke 0xc8dc 1
	#second main stone
	poke 0xc8e0 1
	#thirt main stone
	poke 0xc8e4 1
	#fouth main stone
	poke 0xc8e8 1
	#fifth main stone
	poke 0xc8ec 1

	#bifi's tip for the combo with game master
	poke 0xc110 1

	after time 2 trainer_firebird
}

proc trainer_rambo {} {
	#life
	poke 0xe811 24
	#arrows
	poke 0xe813 10
	#machine gun
	poke 0xe814 10
	#handgranates
	poke 0xe815 10
	#bazooka
	poke 0xe816 10
	#food
	poke 0xe812 6
	poke 0xe817 24
	#activate all weapons
	poke 0xe80e 255
after time 2 trainer_rambo
}

proc trainer_higemaru {} {
	#999900 points
	poke 0xe515 0x99
	poke 0xe516 0x99
	#enemies to kill before entering the gate to a boss
	poke 0xe539 0
	#get all keys
	poke 0xe517 255
	#get all items
	poke 0xe519 255
	poke 0xe51a 255
	#untouchable
	poke 0xe026 255
after time 1 trainer_higemaru
}

proc trainer_bombaman {} {
	#lives
	poke 0x226b 9
	# amount of bombs you can place
	poke 0x1fec 2
	# bomb power (increase the value at your own risk :p)
	poke 0x1fee 5
	# if you have more then one powerfull bomb increase the detonation time
	#poke 0x1dc5 3
	# time
	#poke 0x18a2 58

	after time 2 trainer_bombaman
}

proc trainer_msxlogo {} {
	#just for fun
	poke 0xf55e 1
	after frame trainer_msxlogo
}

proc trainer_psychoworld {} {
	#life
	poke 0xa120 100
	#esp
	poke 0xa122 100
	#special modes
	# 0=power mode
	# 1=
	# 2=normal mode
	# 3=max up mode
	# 4=
	# 5=extra mode

	#lets go for power mode (not sure if it works push f4 on start screen of stage 1)
	poke 0xd40d 0
	#unlimited power ups (use 1 trough 9)
	poke 0xd401 255

	#get all weapons
	poke 0xa212 255
	#normal schot power up
	poke 0xa213 9
	#ice shot power up
	poke 0xa214 5
	#fire power up
	poke 0xa215 5
	#sonic shot power up
	poke 0xa216 5

	after time 2 trainer_psychoworld
}

proc trainer_strategicmars {} {
	#money maxed out
	poke 0xc33e 255
	poke 0xc33f 255
	#energy
	poke 0xc1bc 14
	#schield
	poke 0xc1bb 14

	after time 1 trainer_strategicmars
}

proc trainer_gallforce {} {
	#20 hits
	poke 0xccee 15
	poke 0xccec 15
	#all galls
	poke 0xc447 255
	poke 0xcb12 7
	after time 2 trainer_gallforce
}

proc trainer_beamrider {} {
	#unlimited bombs
	poke 0xe22c 99
	#lives
	poke 0xe223 12
	after time 5 trainer_beamrider
}

proc trainer_happyfret {} {
	#unlimited power
	poke 0xbea0 41
	after time 1 trainer_happyfret
}

proc trainer_comeonpicot {} {
	#jean lives
	poke 0xe008 4
	#jean power
	poke 0xeb70 100
	#picot power
	poke 0xec43 250
	after time 2 trainer_comeonpicot
}

proc trainer_hero {} {
	#unlimited power/time
	poke 0xc174 100
	#unlimted bombs
	poke 0xc032 3
	#unlimited lives
	poke 0xc031 4
	after time 5 trainer_hero
}

proc trainer_pastfiner {} {
	#keep radiation low
	poke 0xe126 1
	#lives
	poke 0xe004 10
	after time 10 trainer_pastfiner
}

proc trainer_terramex {} {
	#unlimited lives
	poke 0x5b94 25
	after time 10 trainer_terramex
}

proc trainer_eindeloos {} {
	#unlimited lives
	poke 0x9c91 99

	#big enemy 1 y-pos
	poke 0x9486 200
	#big enemy 2 y-pos
	poke 0x9482 200
	#big enemy 3 y-pos
	poke 0x948a 200
	after time 0.5 trainer_eindeloos
}

proc trainer_mobileplanet {} {
	#invisible
	poke 0xe33a 1
	#stars
	poke 0xe054 0x99
	#lives
	poke 0xe001 0x99
	after time 1 trainer_mobileplanet
}

proc trainer_holeinonespecial {} {
	#always have hole in one (very lame )
	poke 0xc0da 1
	after time 2 trainer_holeinonespecial
}

proc trainer_woodypoco {} {
	#power
	poke 0xe777 251
	poke 0xe778 255
	poke 0xe779 251
	poke 0xe77a 255
	#money
	poke 0xe77f 255
	poke 0xe780 255
	#shot
	poke 0xe7b9 255
	poke 0xe7ba 255
	after time 2 trainer_woodypoco
}

proc trainer_superrambospecial {} {
	#power
	poke 0xc155 255
	#handgun bullets
	poke 0xc165 255
	#arrows
	poke 0xc166 255
	#shotgun bullets
	poke 0xc167 255
	#explosive arrows
	poke 0xc168 255
	#hand granades
	poke 0xc169 255
	#bazooka
	poke 0xc16a 255
	#flowers
	poke 0xc16c 255
	#keys
	poke 0xc16b 255
	#summon sidekick
	poke 0xc218 1
	#prevent sidekick from screaming and moaning
	poke 0xc216 255
	poke 0xc225 255
	after time 2 trainer_superrambospecial
}

proc trainer_supertriton {} {
	#exp
	poke 0xd023 255
	 #life
	poke 0xd024 255
	#red life
	poke 0xd025 255
	after time 1 trainer_supertriton
}

proc trainer_triton {} {
	#exp
	poke 0xe43b 100
	 #life
	poke 0xe439 99
	#magic balls
	poke 0xe43c 99
	after time 1 trainer_triton
}

proc trainer_afterburner {} {
	#missles
	poke 0x5e23 255
	 #lives
	poke 0x5e4e 100
	after time 10 trainer_afterburner
}

proc trainer_iremkarate {} {
	#power
	poke 0xecad 255
	#time
	poke 0xecc6 0x2
	after time 10 trainer_iremkarate
}

proc trainer_rambo3 {} {
	#life
	poke 60ee 0
	after time 2 trainer_rambo3
}

proc trainer_yiearkungfu2 {} {
	#power player 1
	poke 0xe100 32
	#invincible for enemy (bosses only)
	poke 0xe29e 255
	#lives
	poke 0xe055 0x99
	#kill enemy with one hit
	poke 0xe102 1
	after time 5 trainer_yiearkungfu2
}

proc trainer_ashiguine1 {} {
	#energy
	poke 0xe030 255
	after time 2 trainer_ashiguine1

}

proc trainer_pineaplin {} {
	#lives
	poke 0xcc66 99
	#energy
	poke 0xcc62 255
	poke 0xcc64 255
	#level
	poke 0xc023 9
	poke 0xc024 9
	poke 0xc025 9

	after time 2 trainer_pineaplin
}

proc trainer_godzilla {} {
	#lives
	poke 0xe30f 101
	#energy
	poke 0xe336 255
	after time 2 trainer_godzilla
}

proc trainer_backtothefuture {} {
	#lives
	poke 0xf235 99
	#time
	poke 0xf232 13
	after time 10 trainer_backtothefuture
}

proc trainer_bombermanspecial {} {
	#bomb strength
	poke 0xd015 255
	#max bombs
	poke 0xd014 8
	#lives
	poke 0xd00e 99
	#time
	poke 0xd020 199
	#detonata bombs pushing z
	poke 0xd018 1
	#walk faster
	poke 0xd00b 5
	poke 0xd016 2
	#walk trough bombs
	poke 0xd017 1
	#unvulnerble
	poke 0xd01b 255
	after time 10 trainer_bombermanspecial
}
proc trainer_bomberking {} {
	#energy
	poke 0xc5c1 236
	#stuff
	poke 0xc0b2 1
	poke 0xc0c4 1
	poke 0xc56b 1
	poke 0xc571 1
	after time 10 trainer_bomberking
}

proc trainer_gularve {} {
	#energy
	poke 0xe2ad 255
	#weapon number
	poke 0xe2a7 8
	after time 10 trainer_gularve
}

proc trainer_xak2 {} {
	#max life
	poke 0x6dfe 255
	poke 0x6dff 255	
	#money
	poke 0x6e14 255
	poke 0x6e15 255
	after time 10 trainer_xak2
}

proc trainer_bozos_big_adventure {} {
	#life
	poke 0x5544 100
	after time 1 trainer_bozos_big_adventure
}

proc trainer_alifem36 {} {
	#life
	poke 0xc527 99
	poke 0xd3c2 99
	#exp up after killing one enemy
	poke 0xc532 1
	poke 0xc533 0
	poke 0xd3b2 1
	poke 0xd3b3	0
	#money
	poke 0xc530 255
	poke 0xc531 255
	poke 0xd3b0 255
	poke 0xd3b1	255
	#laser shots
	poke 0xc52b 255
	poke 0xd3b4 255

	after time 2 trainer_alifem36
}

proc trainer_dragonslayer6 {} {
	#gold
	poke 0x208c 255
	poke 0x208d 255
	poke 0x208e 255
	# life selios
	poke 0x2304 0x0f
	poke 0x2305 0x27
	#max life selios
	poke 0x2306 0x0f
	poke 0x2307 0x27
	#magic selios
	poke 0x2308 0x0f
	poke 0x2309 0x27
	#max magic selios
	poke 0x230a 0x0f
	poke 0x230b 0x27
	#experience selios (max exp)
	poke 0x230c 255
	poke 0x230d 255
	poke 0x230e 255

	#experience runan (max exp)
	poke 0x234c 255
	poke 0x234d 255
	poke 0x234e 255
	# life runan
	poke 0x2344 0x0f
	poke 0x2345 0x27
	#max life runan
	poke 0x2346 0x0f
	poke 0x2347 0x27
	#magic runan
	poke 0x2348 0x0f
	poke 0x2349 0x27
	#max magic runan
	poke 0x234a 0x0f
	poke 0x234b 0x27

	# life ro
	poke 0x2384 0x0f
	poke 0x2385 0x27
	#max life ro
	poke 0x2386 0x0f
	poke 0x2387 0x27
	#experience ro (max exp)
	poke 0x238c 255
	poke 0x238d 255
	poke 0x238e 255
	#magic ro
	poke 0x2388 0x0f
	poke 0x2389 0x27
	#max magic ro
	poke 0x238a 0x0f
	poke 0x238b 0x27

	# life gale
	poke 0x23c4 0x0f
	poke 0x23c5 0x27
	#max life gale
	poke 0x23c6 0x0f
	poke 0x23c7 0x27
	#experience gale (max exp)
	poke 0x23cc 255
	poke 0x23cd 255
	poke 0x23ce 255
	#magic gale
	poke 0x23c8 0x0f
	poke 0x23c9 0x27
	#max magic gale
	poke 0x23ca 0x0f
	poke 0x23cb 0x27
	after time 60 trainer_dragonslayer6
}

proc trainer_ys3 {} {
	#swords
	poke 0x7fa3 255
	#armor
	poke 0x7fa5 255
	#shields
	poke 0x7fa7 255
	#rings
	poke 0x7fa9 255
	#items
	poke 0x7fab 255
	#ring power
	poke 0x7ead 255
	#experience
	poke 0x7fa0 255
	poke 0x7fa1 255
	#gold
	poke 0x7f9e 255
	poke 0x7f9f 255
	#life
	poke 0x7f97 255
	after time 10 trainer_ys3
}

proc trainer_catboy {} {
	#big cat
	poke 0xe31d 255
	#lives
	poke 0xe00b 0x99
	after time 1 trainer_catboy
}

proc trainer_bastard {} {
	#money
	poke 0xb5f6 255
	#life
	poke 0xb608 255
	after time 1 trainer_bastard
}

proc trainer_alestegaiden {} {
	#lives
	poke 0xd080 99
	#invincible
	poke 0xc930 27
	#option 1
	poke 0xc820 255
	poke 0xc822 172
	poke 0xc823 76
	poke 0xc832 14
	#option 2
	poke 0xc840 255
	poke 0xc842 93
	poke 0xc843 77
	poke 0xc852 14
	after frame trainer_alestegaiden
}

proc trainer_pacmania {} {
	#unknown pokes (mandantory though)
	poke 0x9f2d 255
	poke 0xc953 255
	#blue ghost 1
	poke 0xcd60 255
	#blue ghost 2
	poke 0xcd74 255
	#blue ghost 3
	poke 0xcd88 255
	#blue ghost 4
	poke 0xcd9c 255
	#blue ghost 5
	poke 0xcdb0 255
	#blue ghost 6
	poke 0xcdc4 255
	#blue ghost 7
	poke 0xcdd8 255
	#blue ghost 8
	poke 0xcdec 255
	after time 5 trainer_pacmania
}

proc trainer_pacman {} {
	#ghost 1
	poke 0xe230 1
	#ghost 2
	poke 0xe250 1
	#ghost 3
	poke 0xe270 1
	#ghost 4
	poke 0xe290 1
	after time 1 trainer_pacman
}

proc trainer_r-type {} {
	#invincible
	poke 0xe703 1
	#missle
	poke 0xea24 1
	#pods
	poke 0xea29 2
	poke 0xea2f 2
	after time 1 trainer_r-type
}

proc trainer_buckrodgers {} {
	#time
	poke 0xf172 100
	#lives
	poke 0xf16e 255
	#go trough on portal for next level
	poke 0xf171 1
	after time 1 trainer_buckrodgers
}

proc trainer_arkanoid1 {} {
	#always fire
	poke 0xe551 1
	#99 lives
	poke 0xe01d 99

	#ball above bat
	poke 0xe0f6 [expr {[peek 0xe0ce]+16}]

	#thanks for these cheats mars2000you
	#normal ball speed
	poke 0xe255 12
	#magnetic ball
	poke 0xe324 1
	#long bat
	poke 0xe0d7 4
	poke 0xe0d8 14
	poke 0xe0db 12
	poke 0xe0dc 8
	poke 0xe321 2
	poke 0xe550 2
	#open door to next round
	poke 0xe326 1
	#round (0-32)
	#poke 0xe01b 0
	
	after time 0.1 trainer_arkanoid1
}

proc trainer_arkanoid2 {} {
	#always fire
	poke 0xc789 3
	#all destroying ball
	poke 0xe2e6 1
	#infinitive lives
	poke 0xc78a 6
	#prevent ball from going beyond the limit
	if {[peek 0xc020] >183} {
		poke 0xc020 0	
	}
	#ball always above bat
	poke 0xc021 [peek 0xc786]

	after time 0.1 trainer_arkanoid2
}

proc trainer_inspecteurz {} {
	#infinitive lives
	poke 0xe001 0x99
	#bombs
	poke 0xe054 0x99
	#coins
	poke 0xe055 0x99
	#stay in state
	poke 0xfca2 255
	#blink yellow
	poke 0xe31a 2
	after time 1 trainer_inspecteurz
}

proc trainer_1942 {} {
	#infinitive lives
	poke 0xed2f 9
	#infinitive loops
	poke 0xee81 9
	#get big shot and become invincible
	poke 0xee83 255
	#stage
	#!poke 0xed20 x
	after time 1 trainer_1942
}

proc trainer_boulderdash1 {} {
	#exit is always open
	poke 0xd9b0 1
	#lives
	poke 0xd98f 255
	#unvulnerable
	 poke 0xd83c 0
	after frame trainer_boulderdash1
}

proc trainer_kikikaikai {} {
	#max shot
	poke 0xc070 9
	#lives
	poke 0xc025 255
	after time 2 trainer_kikikaikai
}

proc trainer_feud {} {
	#life full
	poke 0x5885 40
	#burdock
	poke 0x58d5 7
	#ragwort
	poke 0x58d6 7
	#toadflax
	poke 0x58d7 7
	#bones
	poke 0x58d8 7
	#mad sage
	poke 0x58d9 7
	#bog bean
	poke 0x58da 7
	#catsear
	poke 0x58db 7
	#hemlock
	poke 0x58dc 7
	#skullcap
	poke 0x58dd 7
	#feverfew
	poke 0x58de 7
	#mouse tail
	poke 0x58df 7
	#knap weed
	poke 0x58e0 7
	#concoctions
	poke 0x58e1 7
	after time 2 trainer_feud
}

proc trainer_jpwinkle {} {
	#lives
	poke 0xe007 0x98
	#bible
	poke 0xe098 0x98
	#invincible
	poke 0xe0ab 255
	#keys
	poke 0xe032 0x98
	#wings
	poke 0xe0af 255
	#axe
	poke 0xe0b0 30
	poke 0xe0bc 255
	#hamer
	poke 0xe0b1 20
	poke 0xe0bd 255
	#blue lamp
	#poke 0xe0ae 255
	#red lamp
	poke 0xe0ad 255
	#cross
	poke 0xe0ac 255
	after time 2 trainer_jpwinkle
}

proc trainer_jackthenippercoconut {} {
	#lives
	poke 0x8834 9
	#invincible
	poke 0x881d 60
	#do not die while falling
	poke 0x8826 0
	after time 1 trainer_jackthenippercoconut
}
proc trainer_jackthenipper {} {
	#powerbar
	poke 0x2c56 0
	after time 1 trainer_jackthenipper
}

proc trainer_jetsetwilly {} {
	#lives
	poke 0xca1a 17
	after time 1 trainer_jetsetwilly
}

proc trainer_digdug {} {
	#lives
	poke 0xe700 2
	after time 1 trainer_digdug
}

proc trainer_elevatoraction {} {
	#lives
	poke 0xc08a 99
	after time 1 trainer_elevatoraction
}

proc trainer_tankbattalion {} {
	#next level after one kill
	poke 0xe04c 1
	#lives
	poke 0xe04d 0x99
	after time 1 trainer_tankbattalion
}

proc trainer_dynamitedan {} {
	#lives
	poke 0x01f2 9
	after time 5 trainer_dynamitedan
}
proc trainer_raidonbungelingbay {} {
	#energy
	poke 0xe0f5 0
	#bombs
	poke 0xe037 9
	after time 1 trainer_raidonbungelingbay
}

proc trainer_finaljustice {} {
	#energy
	poke 0xe411 100
	after time 1 trainer_finaljustice
}

proc trainer_frontline {} {
	#lives
	poke 0xc001 255
	after time 1 trainer_frontline
}

proc trainer_gyrodine {} {
	#lives
	poke 0xe5e0 10
	after time 30 trainer_gyrodine
}

proc trainer_superboy3 {} {
	#grow big
	poke 0xe190 255
	#invincible
	#!poke 0xe177 255
	#time
	poke 0xe18c 25
	after time 1 trainer_superboy3
}

proc trainer_zaxxon {} {
	#fuel
	poke 0xe176 16
	#lives
	poke 0xe00b 6
	after time 2 trainer_zaxxon
}

proc trainer_carfighter {} {
	#fuel
	poke 0xe080 9
	poke 0xe081 9
	poke 0xe082 9
	#bombs
	poke 0xe30c 0x99
	after time 30 trainer_carfighter
}

proc trainer_aramo {} {
	#exp
	poke 0xc01c 255
	#power
	poke 0xc02b 255
	#knife
	poke 0xc052 255
	#axe
	poke 0xc053 255
	#sword
	poke 0xc054 255
	#shield a
	poke 0xc055 255
	#shield b
	poke 0xc056 255
	#armor a
	poke 0xc057 255
	#armor b
	poke 0xc058 255
	#ring
	poke 0xc059 255
	#lamp
	poke 0xc05a 255
	#jump boots
	poke 0xc05b 255
	#trubo belt
	poke 0xc05c 255
	#jet boots
	poke 0xc05d 255
	#pendant
	poke 0xc05e 255
	#bracelet
	poke 0xc05f 255
	#blaster
	poke 0xc060 255
	#gun
	poke 0xc061 255
	#fire gun
	poke 0xc062 255
	#key
	poke 0xc063 255
	#bottle
	poke 0xc064 255
	#potion
	poke 0xc065 255
	#medicine
	poke 0xc066 255
	after time 1 trainer_aramo
}

proc trainer_hydlide1_msx1 {} {
	#level up after killing one enemy
	poke 0xe004 100
	#power
	poke 0xe002 100
	after time 1 trainer_hydlide1_msx1
}

proc trainer_hydlide1_msx2 {} {
	#level up after killing one enemy
	poke 0xa7be 100
	#power
	poke 0xa7bc 100
	#stats max
	poke 0xa7bd 100
	poke 0xa7bf 9
	poke 0xa7c3 100

	#sword
	poke 0xa7ce 255
	#shield
	poke 0xa7cf 255
	#lamp
	poke 0xa7d0 255
	#cross
	poke 0xa7d1 255
	#water can
	poke 0xa7d2 255
	#tea pot
	poke 0xa7d3 255
	#key
	poke 0xa7d4 255
	#blue christal
	poke 0xa7d5 255
	#pink christal
	poke 0xa7d6 255
	#green christal
	poke 0xa7d7 255

	#fairy 1
	poke 0xa7d8 255
	#fairy 2
	poke 0xa7d9 255
	#fairy 3
	poke 0xa7da 255

	after time 1 trainer_hydlide1_msx2
}

proc trainer_decathlon {} {
	#top speed
	poke 0xe190 255
	after frame trainer_decathlon
}

proc trainer_kingsknight {} {
	#top speed
	poke 0xef44 255
	after frame trainer_kingsknight
}

proc trainer_scarlet7 {} {
	#damage
	poke 0xe106 11
	after time 1 trainer_scarlet7
}

proc trainer_anaza {} {
	#power
	poke 0xc016 32
	#credit
	poke 0xc01e 0x99
	poke 0xc01d 0x99
	after time 1 trainer_anaza
}

proc trainer_hydlide3 {} {
	#current power
	poke 0xd01a 255
	#max life
	poke 0xd018 255
	poke 0xd020 255
	#magic points
	poke 0xd022 255
	poke 0xd023 255
	poke 0xd024 255
	poke 0xd025 255
	#charm
	poke 0xd036 255
	#exp
	poke 0xd039 255
	poke 0xd03a 255
	poke 0xd03b 255
	#attack points
	poke 0xd029 255
	poke 0xd02a 255
	#armor class
	poke 0xd02b 255
	poke 0xd02c 255
	#agility
	poke 0xd02d 255
	poke 0xd02e 255
	#intelligence
	poke 0xd032 255
	#dexterity
	poke 0xd030 255
	#luck
	poke 0xd034 255
	#mindforce
	poke 0xd038 255
	#gold
	#!poke 0xd087 255
	#!poke 0xd088 255
	#!poke 0xd089 255
	#unknown
	poke 0xd019 255
	poke 0xd01b 255
	poke 0xd03d 255
	poke 0xd0a9 255
	after time 2 trainer_hydlide3
}

proc trainer_deepforest {} {
	#power
	poke 0xeb1a 255
 	#money
	poke 0xeb1b 99
	poke 0xeb1c 99
	#untouchable
	#poke 0xea0e 255
	#jump higher
	poke 0xea0d 255
	poke 0xea0f 255
	poke 0xea08 255
 	after time 2 trainer_deepforest
}

proc trainer_volguard {} {
	#power
	poke 0xe392 255
 	after time 1 trainer_volguard
}

proc trainer_gyrodine {} {
	#get parachute when hit
	poke 0xe5e4 250
	#lives
	poke 0xe5e0 10
 	after time 1 trainer_gyrodine
}

proc trainer_leonidas {} {
	#homing shot
	poke 0xe32e 6
	#hearts
	poke 0xe054 0x99
	poke 0xe055 0x99
	#air
	poke 0xe057 64
	#shot
	poke 0xe058 64
	#invincible (partly)
	poke 0xe33a 4
	poke 0xe33b 60
	#lives
	poke 0xe001 0x99
	#invincible
	poke 0xe03a 255
	after time 1 trainer_leonidas
}

proc trainer_skygaldo {} {
	#super explosives and full power
	poke 0xf327 255
	after time 1 trainer_skygaldo
}

proc trainer_starsoldier {} {
	#get a more powerfull shot
	#!poke 0xce84 3
	#invincible
	poke 0xcf20 255
	after time 1 trainer_starsoldier
}

proc trainer_ninjakage {} {
	#shot power up (try 1 trough 7 as a value)
	poke 0xe542 4
	#unlimited magix
	poke 0xe547 255
	#lives
	poke 0xe532 99
	after time 1 trainer_ninjakage
}

proc trainer_firerescue {} {
	#always have water
	poke 0xe62e 1
	after time 1 trainer_firerescue
}

proc trainer_pooyan {} {
	#shoot one wolf (very lame )
	poke 0xe006 1
	after time 2 trainer_pooyan
}

proc trainer_sparkie {} {
	#do not explode when the fuse is on fire
	poke 0xe005 0
	after time 2 trainer_sparkie
}

proc trainer_polarstar {} {
	#do not explode when hit
	poke 0x9d61 128
	#missle is always ready
	poke 0x9d44 1
	after time 1 trainer_polarstar
}

proc trainer_aufwiedersehenmonty {} {
	#lives
	poke 0x8431 255
	#fly without a ticket (activate only when playing the game!)
	poke 0x9387 0
	#invincible to monsters
	poke 0x8456 1
	after time 1 trainer_aufwiedersehenmonty
}

proc trainer_deathwish3 {} {
	#shotgun ammo
	poke 0xa478 99
	#bazooka ammo
	poke 0xa47b 99
	#pistol ammo
	poke 0xa479 99
	#sub machine gun
	poke 0xa47a 99
	#invincible
	poke 0x5b91 1
	after time 1 trainer_deathwish3
}

proc trainer_desolator {} {
	#shield
	poke 0x8d05 255
	#energy
	poke 0x8d06 255
	#lives
	poke 0x8cec 101
	after time 1 trainer_desolator
}

proc trainer_guttblaster {} {
	#cosmic cheat
	poke 0x4038 1
	#2 way shot (to change choose value 0-3)
	poke 0x6a96 3
	after time 1 trainer_guttblaster
}

proc trainer_starwars {} {
	#left base ammo
	poke 0x5bd8 10
	#middle base ammo
	poke 0x5bd9 10
	#right base ammo
	poke 0x5bda 10
	after time 1 trainer_starwars
}

proc trainer_spacecamp {} {
	#left base ammo
	poke 0x5bd8 10
	after time 1 trainer_spacecamp
}

proc trainer_hydefos {} {
	#power
	poke 0xc11c 255
	#hydefos control
	#!poke 0xc040 255
	#speed
	poke 0xc106 16
	#lives
	poke 0xd404 100
	#power
	poke 0xc118 100
	after time 1 trainer_hydefos
}

proc trainer_ninjayoumakor {} {
	#damage
	poke 0xd46d 0
	after time 1 trainer_ninjayoumakor
}

proc trainer_silviana {} {
	#power
	poke 0x8c9d 255
	#gold
	poke 0x8ca1 255
	poke 0x8ca2 255
	after time 1 trainer_silviana
}

proc trainer_exoide-z_area5 {} {
	#power
	poke 0xe1d3 100
	#lives
	poke 0xe00b 0x99
	#invincible red
	poke 0xe30d 255
	#invincible green
	poke 0xe33e 255
	#silver color ship
	poke 0xe313 15
	after time 1 trainer_exoide-z_area5
}

proc trainer_theseus {} {
	#power
	poke 0xede3 0x9
	poke 0xede2 0x99
	#time
	poke 0xeddc 0x02
	after time 1 trainer_theseus
}

proc trainer_monstersfair {} {
	#power
	poke 0xd7e2 50
	#give motha some balls
	poke 0xd7cb 99
	#mothas
	poke 0xd81c 9
	after time 1 trainer_monstersfair
}

proc trainer_jagur {} {
	#power
	poke 0xec00 99
	#money
	poke 0xe020 255
	poke 0xe021 127
	after time 1 trainer_jagur
}

proc trainer_heaven {} {
	#life
	poke 0xe038 0x99
	poke 0xe039 0x99
	#defend
	poke 0xe03a 0x99
	poke 0xe03b 0x99
	#attack
	poke 0xe03c 0x99
	poke 0xe03d 0x99
	#money
	poke 0xe03e 0x99
	poke 0xe03f 0x99
	after time 1 trainer_heaven
}

proc trainer_digitaldevil {} {
	#life
	poke 0xd271 255
	after time 1 trainer_digitaldevil
}

proc trainer_gradius3 {} {
	#lives
	poke 0xe360 0x99
	#stage (1-11)
	#poke 0xe361 1
	#only for stage 4
	#poke 0xe363 0
	#set speed to 4
	poke 0xe36d 4
	#all weapons and upgrades (find)
	poke 0xe36f 7
	#choose option (1-3)
	poke 0xe37e 3
	#choose shield (1-2)
	poke 0xe37f 2
	#choose vixen (0-3)
	poke 0xe380 3
	#red map
	poke 0xe393 1
	#blue map
	poke 0xe394 1
	#green map
	poke 0xe395 1
	#spacefighter shield
	poke 0xe396 1
	#extra sensory device	
	poke 0xe397 1
	#activate hard
	#poke 0xe39b 0
	#poke 0xe39d 1
	#activate find
	poke 0xe39c 1
	#activate good
	poke 0xe39d 2
	#activate expand
	poke 0xe39e 1
	#shield on 1=off 3=on
	poke 0xe600 3
	#options
	poke 0xe608 2
	poke 0xe610 1
	poke 0xe620 2
	#shoot or laser (1-12)
	# 1 = normal shoot 2 = back shoot
	# 3 = up shoot 4 = down shoot
	# 5 = laser
	# 6 = meteor laser
	# 7 = screw laser
	# 8 = extended blaster
	# 9 = vector laser
	# 10 = ripple laser
	# 11 = fire blaster
	# 12 = big fire blaster
	poke 0xe630 7
	#extended shoot or laser (1-4;13-14)
	# 1 = normal beam 2 = tail beam
	# 3 = up double 4 = down double
	# 13 = up laser 14 = down laser
	poke 0xe631 13
	#missile (16-20)
	# 16 = normal 17 = photon 18 = napalm
	# 19 = guided 20 = hawkwind
	poke 0xe632 19
	# double way missile (with vixen=3)
	poke 0xe642 16
	#x-pos enemy
	#poke 0xe806 0
	#x-pos enemy
	#poke 0xe846 0
	#x-pos enemy
	#poke 0xe886 0
	#x-pos enemy
	#poke 0xe8c6 0
	#x-pos enemy
	#poke 0xe906 0
	#x-pos enemy
	#poke 0xe946 0
	#x-pos enemy
	#poke 0xe986 0
	#x-pos enemy
	#poke 0xe9c6 0

	after time 0.5 trainer_gradius3
}

proc trainer_alien8 {} {
	#counter to 9999
	poke 0xd83a 136
	poke 0xd839 136
	poke 0xd838 136
	poke 0xd837 136
	#lives
	poke 0xd81b 10
	after time 1 trainer_alien8
}

proc trainer_strangeloop {} {
	#patches
	poke 0xc48f 99
	#charges
	poke 0xc48d 99
	poke 0xc48e 99
	after time 1 trainer_strangeloop
}

proc trainer_batman {} {
	#lives
	poke 0x19dc 0x99
	#shield
	poke 0x19db 0x99
	#jump
	poke 0x19da 0x99
	#elec bolt
	poke 0x19d9 0x99
	#get all items
	poke 0x19d8 255
	after time 1 trainer_batman
}

proc trainer_headoverheals {} {
	#lives player 1
	poke 0x2242 0x99
	#shield player 1
	poke 0x2240 0x99
	#elec bolt player 1
	poke 0x223d 0x99
	#ammo player 1
	poke 0x2243 0x99

	#lives player 2
	poke 0x2241 0x99
	#shield player 2
	poke 0x223f 0x99
	#jump player 2
	poke 0x223e 0x99
	after time 1 trainer_headoverheals
}

proc trainer_nightshade {} {
	#power
	poke 0xd04f 3
	after time 1 trainer_nightshade
}

proc trainer_highwaystar {} {
	#power
	poke 0xe20d 255
	poke 0xe20e 5
	#cars
	poke 0xe211 0x0b
	after time 1 trainer_highwaystar
}

proc trainer_pitfall1 {} {
	#power (6789ab)
	poke 0xe1d6 9
	poke 0xe1d7 9
	poke 0xe1d8 9
	poke 0xe1d9 9
	poke 0xe1da 9
	poke 0xe1db 9
	#lives
	poke 0xe012 255
	after time 1 trainer_pitfall1
}

proc trainer_gunfright {} {
	#lives
	poke 0xd05e 3
	#bullets
	poke 0xd05d 5
	#money
	poke 0xd052 99
	#invincible (player dissapears)
	#!poke 0xd078 1
	after time 1 trainer_gunfright
}

proc trainer_knightlore {} {
	#lives
	poke 0xd81b 9
	#days spend
	poke 0xd81a 0
	after time 1 trainer_knightlore
}

proc trainer_pitfall2 {} {
	 if {[peek 0xe05d] != 9} {
		 poke 0xe050 0
	 }
	#power (89abcd)
	poke 0xe058 9
	poke 0xe059 9
	poke 0xe05a 9
	poke 0xe05b 9
	poke 0xe05c 9
	poke 0xe05d 9
	after time 1 trainer_pitfall2
}

proc trainer_predator {} {
	#power
	poke 0xc207 255
	#mines
	poke 0xc230 9
	poke 0xc231 9
	#handgrenade
	poke 0xc235 9
	poke 0xc236 9
	#bullet
	poke 0xc232 9
	poke 0xc233 9
	poke 0xc234 9
	after time 1 trainer_predator
}

proc trainer_runemaster1 {} {
	#money
	poke 0x61e2 0x0f
	poke 0x61e3 0x27
	#player 1 power
	poke 0x61dc 255
	after time 1 trainer_runemaster1
}

proc trainer_mastersoftheuniverse {} {
	#life he-man
	poke 0xcb01 90
	#time
	poke 0xd23c 9
	poke 0xd23e 9
	poke 0xd23f 9
	#have all cords
	poke 0xccd4 255
	#lives
	poke 0xa4e3 9
	after time 1 trainer_mastersoftheuniverse
}

proc trainer_thingbouncesback {} {
	#oil level
	poke 0x8e4e 180
	after time 1 trainer_thingbouncesback
}

proc trainer_bounder {} {
	#lives
	poke 0x96f 101
	after time 10 trainer_bounder
}

proc trainer_fruityfrank {} {
	#lives
	poke 0x4144 4
	after time 10 trainer_fruityfrank
}

proc trainer_batmanthemovie {} {
	#life
	poke 0x613d 255
	#time
	poke 0x5dac 58
	poke 0x5daa 58
	after time 1 trainer_batmanthemovie
}

proc trainer_venomstrikesback {} {
	#lives
	poke 0x2bb1 255
	poke 0x2bb2 255
	#power slot 1 filled with penetrator
	poke 0x2c33 1
	poke 0x2c34 0x99
	#power slot 2 filled with lifter
	poke 0x2c38 6
	poke 0x2c39 0x99
	#power slot 3 filled with jack rabbit
	poke 0x2c3d 5
	poke 0x2c3e 0x99
	after time 1 trainer_venomstrikesback
}

proc trainer_alehop {} {
	#mood (stay happy)
	poke 0xdb68 4
	#time
	poke 0xdb5d 9
	poke 0xdb5e 9
	poke 0xdb5f 1
	after time 10 trainer_alehop
}

proc trainer_policeacademy1 {} {
	#65535 bullits
	poke 0x4d39 255
	poke 0x4d3a 255
	after time 1 trainer_policeacademy1
}

proc trainer_policeacademy2 {} {
	#health
	poke 0xc006 0
	#mistakes
	poke 0xc007 0
	#bullits
	poke 0xc003 255
	after time 1 trainer_policeacademy2
}

proc trainer_ttracer {} {
	#temp
	poke 0x5ad9 255
	after time 1 trainer_ttracer
}

proc trainer_humphrey {} {
	#lives
	poke 0x6797 9
	after time 10 trainer_humphrey
}

proc trainer_arkos1 {} {
	#lives
	poke 0x5bf2 10
	after time 10 trainer_arkos1
}

proc trainer_arkos2 {} {
	#lives
	poke 0x5bf2 10
	after time 10 trainer_arkos2
}

proc trainer_arkos3 {} {
	#lives
	poke 0x5bf2 10
	after time 10 trainer_arkos3
}

proc trainer_afterthewar1 {} {
	#power
	poke 0xba9a 16
	after time 1 trainer_afterthewar1
}

proc trainer_satan1 {} {
	#power unlimited
	poke 0x8462 0
	#some gauge
	poke 0xc88f 100
	after time 1 trainer_satan1
}

proc trainer_indianajonesandthelastcrusade {} {
	#power
	poke 0x7ac0 255
	#whip
	poke 0x7abe 10
	#lives/vidas
	poke 0x7ac1 9
	after time 1 trainer_indianajonesandthelastcrusade
}

proc trainer_indianajonesandthetempleofdoom {} {
	#lives
	poke 0xc234 59
	after time 1 trainer_indianajonesandthetempleofdoom
}

proc trainer_corsarios1 {} {
	#power
	poke 0x96f2 99
	after time 1 trainer_corsarios1
}

proc trainer_corsarios2 {} {
	#power
	poke 0x979b 9
	after time 1 trainer_corsarios2
}

proc trainer_greenberret {} {
	#lives
	poke 0xf120 57
	#have bazooka
	poke 0xf0c3 4
	after time 1 trainer_greenberret
}

proc trainer_discwarrior {} {
	#power
	poke 0x8513 255
	after time 1 trainer_discwarrior
}

proc trainer_eidelon {} {
	#power
	poke 0x012f 16
	after time 1 trainer_eidelon
}

proc trainer_hardboiled {} {
	#power
	poke 0xd047 32
	after time 1 trainer_hardboiled
}

proc trainer_battlechopper {} {
	#power
	poke 0x8335 0
	after time 1 trainer_battlechopper
}

proc trainer_tnt {} {
	#lives (bootleg version)
	#poke 0x7074 100
	#ammo (bootleg version)
	#poke 0x4894 255

	#lives
	poke 0x320e 255
	#ammo
	poke 0x489d 255
	after time 1 trainer_tnt
}

proc trainer_goody {} {
	#invincible
	poke 0xaeaf 8
	after time 10 trainer_goody
}

proc trainer_alpharoid {} {
	#enemy dies after one kick
	poke 0xe8f7 0
	poke 0xeb0e 231
	#invincible (in space)
	poke 0xe399 4
	#lives
	poke 0xe3a2 98
	after time 1 trainer_alpharoid
}

proc trainer_operationwolf {} {
	#invincible
	poke 0x9ae9 0
	#grenades
	poke 0x9ae6 9
	#bullits
	poke 0xa316 32
	#mag
	poke 0x9ae7 9
	after time 2 trainer_operationwolf
}

proc trainer_teenageninjamutantheroturtles {} {
	#invincible
	poke 0x5f66 16
	after time 1 trainer_teenageninjamutantheroturtles
}

proc trainer_bladelords {} {
	#lives
	poke 0x0430 0x99
	#invincible
	poke 0x0429 255
	#have blades
	poke 0x0424 255
	after time 1 trainer_bladelords
}

proc trainer_chiller {} {
	#power
	poke 0x9ab8 25
	poke 0x9ab9 0
	after time 1 trainer_chiller
}

proc trainer_crusader {} {
	#power
	poke 0xe491 255
	#get sword
	poke 0xe471 1
	after time 1 trainer_crusader
}

proc trainer_dokidokipenguinland {} {
	#eggs
	poke 0xe111 100
	after time 1 trainer_dokidokipenguinland
}

proc trainer_pegasus {} {
	#power
	poke 0xc006 43
	#stage (change value to get that stage)
	#!poke 0xc007 1
	#big pegasus
	poke 0xc009 4
	after time 1 trainer_pegasus
}

proc trainer_avenger {} {
	#green sun
	poke 0xbdf8 255
	#red sun
	poke 0xbdf9 255
	#do not die
	#!poke 0xbdfa 0
	#shuriken
	poke 0xacd5 255
	#keys
	poke 0xacd3 255
	after time 1 trainer_avenger
}

proc trainer_randar3 {} {
	#life player 1
	poke 0xd014 0x3
	poke 0xd015 0xe7
	#magic player 1
	poke 0xd016 0x3
	poke 0xd017 0xe7

	#life mick
	poke 0xd074 0x3
	poke 0xd075 0xe7
	#magic mick
	poke 0xd076 0x3
	poke 0xd077 0xe7

	#money
	poke 0xd185 254
	poke 0xd186 255
	poke 0xd187 255

	after time 1 trainer_randar3
}

proc trainer_battennodaibouken  {} {
	#power
	poke 0xe2ed 128
	after time 1 trainer_battennodaibouken
}

proc trainer_leatherskirts  {} {
	#energy
	poke 0xb406 255
	after time 1 trainer_leatherskirts
}

proc trainer_blackcyclon  {} {
	#lives
	poke 0xd15a 8
	#power
	poke 0xd13e 255
	after frame trainer_blackcyclon
}

proc trainer_casanova {} {
	#lives
	poke 0xb489 255
	after time 1 trainer_casanova
}

proc trainer_chubbygristle {} {
	#lives
	poke 0x7532 255
	after time 1 trainer_chubbygristle
}

proc trainer_darwin4078 {} {
	#max weapons
	poke 0xe8e6 144
	poke 0xe8f0 135
	poke 0xe9bb 144
	after time 1 trainer_darwin4078
}

proc trainer_demonia {} {
	#max weapons
	poke 0x806c 255
	after time 1 trainer_demonia
}

proc trainer_jaws {} {
	#max hits
	poke 0x84a7 128
	#invincible
	poke 0x84a5 255
	#time
	poke 0x806c 255
	#lives
	poke 0x8078 0x99
	after time 1 trainer_jaws
}
proc trainer_joeblade {} {
	#max hits
	poke 0x8d97 128
	#let enemies think you are one of them
	poke 0x8dbe 1
	#ammo
	poke 0x8d98 255
	#keys
	poke 0x8d8e 57
	poke 0x8d8f 57
	#bombs
	poke 0x8d90 57
	poke 0x8d91 57
	#bomb defusion timer
	poke 0x8dc0 41
	after time 1 trainer_joeblade
}

proc trainer_drarchie {} {
	#life
	poke 0xd3c8 64
	#gold
	poke 0x404 0x99
	poke 0x405 0x99
	poke 0x406 0x99
	#exp
	poke 0xd3c9 60
	after time 1 trainer_drarchie
}

proc trainer_ghostbusters {} {
	#lives
	poke 0xefa8 0x99
	poke 0xefa9 0x99
	#tries to get past the mars mellow man
	poke 0xf13a 255
	#fast time
	poke 0xefe9 0x99
	#ghost x-position
	poke 0xf0ed 128
	#ghost y-position
	poke 0xf0ee 160
	after time 0.10 trainer_ghostbusters
}

proc trainer_replicart {} {
	#this trainer has been made with bifi's ips patch he deserves the credit :)
	poke 0xc0ad 1
	poke 0xc20c 33
	poke 0xc4b8 1
	poke 0xc4bb 255
	poke 0xf1a3 255
	poke 0xf1a4 255
	poke 0xf1a5 255
	poke 0xf1a6 255
	poke 0xf1a9 5
	poke 0xfbf5 31
	poke 0xfbf7 28
	poke 0xfbf8 31
	#freeze time before chistal moves
	poke 0xc182 53
	poke 0xc181 53
	after time 1 trainer_replicart
}

proc trainer_dass {} {
	#!!!
	#special power
	poke 0xaea1 162
	#invincible
	poke 0x9abb 255
	#lives
	poke 0xae51 4
	after time 1 trainer_dass
}

proc trainer_mirai {} {
	#special power
	poke 0xaea1 162
	after time 1 trainer_mirai
}

proc trainer_sonyc {} {
	#99 rings... hooray :p
	poke 0xd311 0x99
	after time 1 trainer_sonyc
}

proc trainer_brotheradventure {} {
	#3 lives
	poke 0xe812 3
	after time 1 trainer_brotheradventure
}

proc trainer_tensairabbiandaifunsen {} {
	#lives
	poke 0xec23 255
	#timer
	poke 0xec27 255
	after time 10 trainer_tensairabbiandaifunsen
}

proc trainer_madrider {} {
	#timer
	poke 0xc510 59
	#full speed
	#!poke 0xc577 255
	after time 1 trainer_madrider
}

proc trainer_castle_of_blackburn {} {
	#lives
	poke 0xda32 99
	#unlimited swords
	poke 0xda2c 99
	#invincible
	poke 0xda0e 255
	after time 10 trainer_castle_of_blackburn
}

proc trainer_daiva5 {} {
	#battle ships in stock
	poke 0xd0b8 99
	#o.m in stock
	poke 0xd0cc 98
	#missle ships in stock
	poke 0xd0d6 97
	#cruisers in stock
	poke 0xd0c2 99

	#damage on planets
	poke 0x950a 0
	poke 0x950b 0

	#stop timer on planets
	poke 0x94ee 0

	#money
	poke 0xd7ab 255
	poke 0xd7ac 255

after time 1 trainer_daiva5
}

proc trainer_daiva4 {} {
	#all kind of ships in stock
	poke 0xc4ee 99
	poke 0xc4ef 99
	poke 0xc4f0 99
	poke 0xc4f1 99

	#life on planet
	poke 0xd408 255

	after time 10 trainer_daiva4
}

proc trainer_pennantrace1 {} {
	#3 out (both teams)
	poke 0xe19a 3
	#player 2 always 0 points (end total)
	poke 0xe196 0
	#always have 2 strikes
	poke 0xe198 2
}

proc trainer_princessmaker {} {
	#inf gold
	poke 0x1c96 255
	poke 0x1c97 255
	#0 fatigue
	poke 0x1c3c 0
	after time 1 trainer_princessmaker
}

proc trainer_magunam {} {
	#ammo
	#poke 0xe048 255
	#trigger timer
	poke 0xe062 4
	#lives
	poke 0xe043 9
	after time 1 trainer_magunam
}

proc trainer_mashou_no_yakata_gabalin {} {
	#invincible
	poke 0xc0b8 255
	after time 1 trainer_mashou_no_yakata_gabalin
}

proc trainer_iceworld {} {
	#lives
	poke 0xe00b 5
	after time 10 trainer_iceworld
}
proc trainer_manes {} {
	#lives
	poke 0xdb93 5
	after time 10 trainer_manes
}

proc trainer_issunhoushi_no_donnamondai  {} {
	#power
	poke 0xe1b5 64
	#invincible
	poke 0xe03a 255
	after frame trainer_issunhoushi_no_donnamondai
}

proc trainer_scarlet7 {} {
	#power
	poke 0xe106 10
	after time 1 trainer_scarlet7
}

proc trainer_zoom909 {} {
	#fuel
	poke 0xe021 0
	after time 1 trainer_zoom909
}

proc trainer_finalfantasy {} {
	#gold
	poke 0xc255 255
	poke 0xc256 255

	#life player 1
	poke 0xc00a 99
	poke 0xc008 255
	poke 0xc009 255

	#life player 2
	poke 0xc04a 99
	poke 0xc048 255
	poke 0xc049 255

	#life player 3
	poke 0xc08a 99
	poke 0xc088 255
	poke 0xc089 255

	#life player 4
	poke 0xc0ca 99
	poke 0xc0c8 255
	poke 0xc0c9 255
	after time 1 trainer_finalfantasy
}

proc trainer_breaker {} {
	#balls
	poke 0x953d 99
	#ball under bat
	poke 0x922a [expr {[peek 0x9236]+8}]
	#ball at same height as 2nd bat
	poke 0x922c [expr {[peek 0x9237]-40}]
	after frame trainer_breaker
}

proc trainer_bosconian  {} {
	#life bar
	poke 0xe00d 99
	#'attack' alarm doesn't sound (makes it harder)
	poke 0xe810 0
after time 1 trainer_bosconian
}

proc trainer_zaider  {} {
	#damage bar
	poke 0xe1dc 0
	#psyco-g1 ammo
	poke 0xe09b 255
	#psyco-g2 ammo
	poke 0xe09c 255
	#psyco-g3 ammo
	poke 0xe09d 255
	#zaider damage
	poke 0xe0b5 0
after time 1 trainer_zaider
}

proc trainer_fray  {} {
	#life bar full
	poke 0x2010 200
	#money 65535
	poke 0x2289 255
	poke 0x228a 255
	#auto big shot (hold c to hold)
	poke 0x2286 49

	#scepter 1
	poke 0x2295 2
	#scepter 2
	poke 0x2296 2
	#scepter 3
	poke 0x2297 2
	#scepter 4
	poke 0x2298 2
	#scepter 5
	poke 0x2299 2
	#scepter 6
	poke 0x229a 2
	#scepter 7
	poke 0x229b 2
	#scepter 8
	poke 0x229c 2

	#medium shield
	poke 0x229e 1
	#iron shield
	poke 0x229f 1
	#big iron shield
	poke 0x22a0 1
	#gold shield
	poke 0x22a1 1

	#slice of bread
	poke 0x22a2 99
	#slice of bacon
	poke 0x22a3 99
	#bottle
	poke 0x22a4 99
	#scroll 1
	poke 0x22a5 99
	#scroll 2
	poke 0x22a6 99
	#scroll 3
	poke 0x22a7 99
	#scroll 4
	poke 0x22a8 99
	#scroll 5
	poke 0x22a9 99
	#scroll 6
	poke 0x22aa 99
	#scroll 7
	poke 0x22ab 99
	#scroll 8
	poke 0x22ac 99
	#scroll 9
	poke 0x22ad 99
	#scroll 10
	poke 0x22ae 99
	#scroll 11
	poke 0x22af 99

after time 1 trainer_fray
}

proc trainer_xzr1  {} {
	#exp max
	poke 0xd000 255
	poke 0xd001 255
	poke 0xd002 255
	#life
	poke 0xd018 255
	#invincible
	poke 0xd0ac 255
	#max money
	poke 0xd035 255
	poke 0xd036 255
	poke 0xd037 255
after time 1 trainer_xzr1
}

proc trainer_xzr2  {} {
	#max money
	poke 0xd180 255
	poke 0xd181 255
	poke 0xd182 255
	#life
	poke 0xd0a0 255
	#exp
	poke 0xd0a8 255
	poke 0xd0a9 255
	after time 1 trainer_xzr2
}

proc trainer_robocop {} {
	#power
	poke 0x75bb 100
	poke 0x7746 255
	#lives
	poke 0x7752 99
	after time 1 trainer_robocop
}

proc trainer_fparodic2 {} {
	#lives
	poke 0xc008 9
after time 1 trainer_fparodic2
}

proc trainer_borfesu {} {
	#life
	poke 0xe37c 255
	#staff
	poke 0xe09f 1
	#bow
	poke 0xe0a0 1
	#whirlwind
	poke 0xe0a1 1
	#boomerang
	poke 0xe0a2 1
	#keltic cross
	poke 0xe0a3 1
	#sword
	poke 0xe0a4 1
	#scepter
	poke 0xe0a5 1
	#ball
	poke 0xe0a6 1
	#statue
	poke 0xe0a7 1
	#space suit
	poke 0xe0a8 1
	#key
	poke 0xe0a9 1
	#pot
	poke 0xe0aa 1
	#ketle
	poke 0xe0ab 1
	#bottle
	poke 0xe0ac 1
	#vase
	poke 0xe0ad 1
	#money
	poke 0xe043 255
	poke 0xe044 255
	#exp
	poke 0xe045 200
	poke 0xe046 255
	#containers
	poke 0xe01a 250
	#invincible
	poke 0xe37d 201
	after time 0.25 trainer_borfesu
}

proc trainer_alcazar {} {
	#power
	poke 0xf082 127
	poke 0xf209 1
after frame trainer_alcazar
}

proc trainer_saimazoom {} {
	#lives
	poke 0xd9cf 9
	#water
	poke 0xd9d8 0x99
	poke 0xd9d9 0x99
	#remove time limit
	poke 0xd9d1 0
	#slot 1
	#poke 0xd9e8 102
	#slot 2
	poke 0xd9e9 102
	#slot 3
	poke 0xd9ea 103
	#slot 4
	poke 0xd9eb 106
	#items
	# 100 = bag
	# 101 = water bottle
	# 102 = knife*
	# 103 = pick axe*
	# 104 = key*
	# 105 = kanoo*
	# 106 = gun*
	# 107 = nothing
	if {[peek 0xd9e8] < 100} {
		 poke 0xd9e8 100
	 }
	
	 poke  0xd9e8 [expr [peek 0xd9e8]+1]
	
	if {[peek 0xd9e8] > 106} {
		 poke 0xd9e8 100
	 }
	after time 1 trainer_saimazoom
}

proc trainer_thecure {} {
	#power
	poke 0xdae6 64
	#invincible
	poke 0xdadf 4
	#get yellow key
	poke 0xdae0 1
	#get white key
	poke 0xdae1 1
	#kill enemy with one hit (end boss 3 caution!)
	#poke 0xdb8d 1
	#hearts
	poke 0xdd44 0x99
	#next stage (-1)
	#poke 0xdd46 1
	#optional weapon (1 holy water/2 daggers)
	poke 0xdae5 2
	#sand of time
	poke 0xdae4 1
	after time 1 trainer_thecure
}

proc trainer_universe_unknown {} {
	#power
	poke 0xee55 128
	poke 0xee54 255
	#lives (ascii code)
	poke 0xf2c4 233
	after frame trainer_universe_unknown
}

proc trainer_caverns_of_titan {} {
	#oxygen
	poke 0xd4e4 255
	#walking speed
	poke 0xd6dc 1
	after frame trainer_caverns_of_titan
}

proc trainer_seikema2special {} {
	#life
	poke 0xe060 255
	poke 0xe061 255
	#money
	poke 0xe062 255
	#weapon (try 0-5)
	poke 0xe048 5
after time 10 trainer_seikema2special
}

proc trainer_moai_no_hibou {} {
	#pillars
	poke 0xe30e 255
	#life
	poke 0xe00b 10
after time 1 trainer_moai_no_hibou
}

proc trainer_kinnikuman {} {
	#life player 1
	poke 0xf002 255
after time 1 trainer_kinnikuman
}

proc trainer_poppaq_the_fish {} {
	#life player 1
	poke 0xe122 0
after time 1 trainer_poppaq_the_fish
}

proc trainer_spaceinvader {} {
	#lives (for the space invader (1984) (taito) version
	poke 0xe046 4
after time 1 trainer_spaceinvader
}

proc trainer_megamitensho {} {
	#power bar
	poke 0xd271 255
	#other bar
	poke 0xd272 255
	#disks
	poke 0xd6bb 99
after time 1 trainer_megamitensho
}

proc trainer_ink {} {
	#lives (more than 6 lives will mess up the gfx, so stay at 5 to be safe)
	poke 0x700c 5
after time 1 trainer_ink
}

proc trainer_penguin_kun_wars1 {} {
	#enemy stuck in one position
	#!poke 0xd020 15
	#enemy k.o
	poke 0xd026 255
after time 1 trainer_penguin_kun_wars1
}

proc trainer_penguin_kun_wars2 {} {
	#enemy k.o after being hit
	poke 0xb126 255
	poke 0xb146 255
	poke 0xb166 255
after time 1 trainer_penguin_kun_wars2
}

proc trainer_seiken_acho {} {
	#power
	poke 0xecad 255
	#time
	poke 0xecc6 0x2
after time 1 trainer_seiken_acho
}

proc trainer_dorodon {} {
	#kill all enemies
	poke 0xc9fb 1
	poke 0xc9fc 255
	#lives
	poke 0xc6f3 5
after time 1 trainer_dorodon
}

proc trainer_mrchin {} {
	#lives
	poke 0xe101 8
	after time 1 trainer_mrchin
}

proc trainer_laptick2 {} {
	#lives
	poke 0xe018 10
	#always have an exit
	poke 0xe020 7
	after time 1 trainer_laptick2
}

proc trainer_kingsballoon {} {
	#lives
	poke 0xe490 255
	after time 60 trainer_kingsballoon
}

proc trainer_grogsrevenge {} {
	#stones collected
	poke 0xe089 255
	poke 0xe08a 255
	#tires left
	poke 0xe08b 5
	after time 30 trainer_grogsrevenge
}

proc trainer_actman {} {
	#made by benoit of bluemsx
	#lives player 1
	poke 0xe2a7 6
	#lives player 2
	poke 0xe2a8 6
	#viewing mode
	#poke 0xe2a9 8
	#weapon 1=sword 2=axe
	poke 0xe1b4 1
	#only one bear
	poke 0xe247 1
	#only one bird
	poke 0xe248 1
	#only one fish
	poke 0xe249 1
	#only one red monster
	poke 0xe24a 1
	#only one snake
	poke 0xe24b 1
	#only one blue monster
	poke 0xe24c 1
	#bonus infinite time
	poke 0xe1e9 20
	after time 1 trainer_actman
}

proc trainer_shoutmatch {} {
	#power
	poke 0xc001 128
	after time 1 trainer_shoutmatch
}

proc trainer_cosmoexplorer {} {
	#fuel
	poke 0xdaa9 255
	#power
	poke 0xdac1 0
	#photon torpedos
	poke 0xdaa7 99
	after time 1 trainer_cosmoexplorer
}

proc trainer_funkymouse {} {
	#lives
	poke 0xe211 1
	after time 1 trainer_funkymouse
}

proc trainer_knitherspecial  {} {
	#life meter
	poke 0xf060 9
	poke 0xf061 9
	poke 0xf062 9
	poke 0xf063 9
	#small keys
	poke 0xf056 9
	#fireballs (bombs)
	poke 0xf050 9
	#thundersword
	poke 0xf051 9
	#firewave
	poke 0xf053 9
	#big key (makes the game boring)
	#!poke 0xf057 9
	#cracker
	poke 0xf052 99
	after time 1 trainer_knitherspecial
}

proc trainer_warpwarp {} {
	#lives
	poke 0xe089 99
	after time 1 trainer_warpwarp
}

proc trainer_iga_ninpouten1 {} {
	#lives
	poke 0xe00b 99
	#time
	poke  0xe080 9
	after time 1 trainer_iga_ninpouten1
}

proc trainer_iga_ninpouten2 {} {
	#lives
	poke 0xe00b 0x99
	after time 1 trainer_iga_ninpouten2
}

proc trainer_sinbad {} {
	#power
	poke 0xead7 14
	#lives
	poke 0xeae3 6
	after time 1 trainer_sinbad
}

proc trainer_starblazer {} {
	#bombs
	poke 0xe415 99
	#lives
	poke 0xe409 99
	after time 1 trainer_starblazer
}

proc trainer_trailblazer {} {
	#jumps left in arcade mode
	poke 0x8721 9
	#time on cb:a9 (leave this value alone, it checks for s)
	poke 0x866c 4
	poke 0x866b 4
	#poke 0x866a 9
	#poke 0x8669 9
	after time 1 trainer_trailblazer
}

proc trainer_vampire {} {
	#power
	poke 0x9491 99
	#lives
	poke 0x9493 8
	after time 1 trainer_vampire
}

proc trainer_pachipro_densetsu {} {
	#money
	poke 0xe230 0x99
	poke 0xe22f 0x99
	poke 0xe22e 0x99
	poke 0xe22d 0x99
	after time 1 trainer_pachipro_densetsu
}

proc trainer_indian_no_bouken {} {
	#lives
	poke 0xe60e 255
	#boomerang
	poke 0xe610 99
	after time 1 trainer_indian_no_bouken
}

proc trainer_shalom {} {
	#power
	poke 0xe3f6 255
	after time 1 trainer_shalom
}

proc trainer_dragonquest2 {} {
	#hp
	poke 0xe63b 255
	poke 0xe63c 255
	#magic
	poke 0xe63d 255
	#gold
	poke 0xe624 255
	poke 0xe625 255
	#max exp
	poke 0xe633 255
	poke 0xe634 255
	poke 0xe635 255
	#max hp
	poke 0xe630 255
	poke 0xe631 255
	#max stats
	poke 0xe636 255
	poke 0xe637 255
	poke 0xe638 255
	poke 0xe639 255
	#level
	#poke 0xe63e 22
	after time 1 trainer_dragonquest2
}

proc trainer_break_in {} {
	#thanks mars2000you
	#infinite lives
	poke 0x7d9e 5
	#always fire
	poke 0x85d5 90
	#long bat
	poke 0x85d6 16
	#open room 2
	poke 0x85ff 28
	poke 0x8600 28
	poke 0x8601 28
	poke 0x8602 28
	#open room 3
	poke 0x871f 28
	poke 0x8720 28
	poke 0x8721 28
	poke 0x8722 28
	#open room 4
	poke 0x883f 28
	poke 0x8840 28
	poke 0x8841 28
	poke 0x8842 28
	#frozen guardian
	poke 0x8a7b 0
	#invisible guardian (hard game !)
	#poke 0x8a83 0
	poke 0x8487 [expr {[peek 0x85d3]+16}]
	after frame trainer_break_in
}

proc trainer_hypersports1 {} {
	#always qualify
	poke 0xe05a 9
	poke 0xe059 0x99
	after time 1 trainer_hypersports1
}

proc trainer_hypersports2 {} {
	#always qualify
	poke 0xe088 0x99
	poke 0xe089 0x99
	#always full power
	poke 0xe101 255
	#unlimited arrows
	#poke 0x111 8
	#freezes time
	#poke 0xe1a6 15
	after time 1 trainer_hypersports2	
}

proc trainer_hypersports3 {} {
	#freeze time
	poke 0xe0d0 0
	#top speed cycling
	poke 0xe0ad 255
	#top speed long jump
	poke 0xe121 255
	after frame trainer_hypersports3
}

proc trainer_hyperolympics1 {} {
	#sprint time
	poke 0xe0a5 0
	poke 0xe0a9 0
	after time 1 trainer_hyperolympics1
}

proc trainer_hyperolympics2 {} {
	#sprint time
	poke 0xe0a5 0
	poke 0xe0a9 0
	after time 1 trainer_hyperolympics1
}

proc trainer_gpworld {} {
	# time
	poke 0xe1cb 0
	# round (1-9)
	#poke 0xe005 1
	# level (1-3)
	#poke 0xe00f 1
	after time 1 trainer_gpworld	
}

proc trainer_deepdungeon1 {} {
	# life
	poke 0xc157 255
	poke 0xc158 255
	#max gold
	poke 0xc160 255
	poke 0xc161 255
	#max exp
	poke 0xc159 255
	poke 0xc15a 255
	after time 1 trainer_deepdungeon1
}

proc trainer_deepdungeon2 {} {
	# life
	poke 0xc17e 255
	poke 0xc17f 255
	#max gold
	poke 0xc18a 255
	poke 0xc18b 255
	#max level
	poke 0xc17d 99
	#max ap
	poke 0xc185 255
	#max ac
	poke 0xc182 255
	#max ag
	poke 0xc184 255
	#max luck
	poke 0xc189 255
	after time 1 trainer_deepdungeon2
}

proc trainer_nsub {} {
	#lives
	poke 0xc13b 3
		#round (0-99)
		#poke 0xc142 1
	after time 1 trainer_nsub
}

proc trainer_kobashi {} {
	#infinite power player 1
	poke 0xd448 50
	#infinite power player 2 or computer 1
	#poke 0xd454 50
	#infinite power computer 2
	#poke 0xd460 50
	after time 1 trainer_kobashi
}

proc trainer_gurulogicmsx1 {} {
	#infinite time
	poke 0xc1e2 0
	#level (0-17)
	#poke 0xc1f2 0
	after time 1 trainer_gurulogicmsx1
}

proc trainer_gurulogicmsx2 {} {
	#infinite time
	poke 0xa68c 17
	#level (0-4)
	#poke 0x9bc5 0
	#poke 0xcc00 0
	after frame trainer_gurulogicmsx2
}

proc trainer_themunsters {} {
	#power
	poke 0xaa7a 255
	after time 1 trainer_themunsters
}

proc trainer_ballout {} {
	#time
	poke 0xaa40 99
	after time 60 trainer_ballout
}

proc trainer_ballout2 {} {
	#time
	poke 0x9aa6 99
	after time 60 trainer_ballout2
}

proc trainer_ballout_special {} {
	#time
	poke 0x9560 99
	after time 60 trainer_ballout_special
}

proc trainer_bankbuster {} {
	#lives
	poke 0x63fd 99
	after time 1 trainer_bankbuster
}

proc trainer_exterlien {} {
	#power
	poke 0x9e2d 0x0f
	poke 0x9e2e 0x27
	#exp
	poke 0x9e33 0xff
	poke 0x9e34 0xff
	after time 1 trainer_exterlien
}

proc trainer_legendlyknight {} {
	#power
	poke 0xd07b 0x1
	poke 0xd07c 0x20
	#invincible
	poke 0xd083 255

	#short sword
	poke 0xe301 1
	#fire arrow
	poke 0xe302 1
	#bible
	poke 0xe303 1
	#magic christal
	poke 0xe304 1
	#thunder
	poke 0xe305 1

	#holy water
	poke 0xe306 1
	#glasses
	poke 0xe307 1
	#ring
	poke 0xe308 1
	#rollers
	poke 0xe309 1
	#armor
	poke 0xe30a 1
	#diving suit
	poke 0xe30b 1
	#key
	poke 0xe30c 1

	#kill end boss with one shot
	poke 0xd08c 1
	after time 1 trainer_legendlyknight
}

proc trainer_chopper2 {} {
	#damage
	poke 0x6a9e 0
	poke 0x6aa2 0
	#heat seeking missle
	poke 0x6397 9
	#tracking missle
	poke 0x6398 9
	#machine gun
	poke 0x6399 232
	poke 0x639a 3
	after time 1 trainer_chopper2
}

proc trainer_itaexpress {} {
	#lives
	poke 0xe308 3
	after time 1 trainer_itaexpress
}

proc trainer_sammyudensetsu {} {
	#lives
	poke 0xe016 9
	#power
	poke 0xe024 5
	after time 15 trainer_sammyudensetsu
}

proc trainer_hydlide2 {} {
	#start points
	poke 0xf10b 0x99
	#life max
	poke 0xe01d 0x99
	#strength max
	poke 0xe022 0x99
	#magic max
	poke 0xe025 0x99
	after time 1 trainer_hydlide2
}

proc trainer_zukkoke_yajikita_onmitsudoutyuu {} {

	#life
	poke 0xe060 0x99
	#something
	poke 0xe061 0x99
	#something else
	poke 0xe062 0x99
	#something
	poke 0xe063 0x99
	poke 0xe065 0x99
	#something
	poke 0xe064 0x9

	#item 1-8
	poke 0xe066 0x9
	poke 0xe067 0x9
	poke 0xe068 0x9
	poke 0xe069 0x9
	poke 0xe06a 0x9
	poke 0xe06b 0x9
	poke 0xe06c 0x9
	poke 0xe06d 0x9
after time 1 trainer_zukkoke_yajikita_onmitsudoutyuu
}

proc trainer_namcof1racing {} {
	#start points
	poke 0xc701 11
	#damage
	poke 0xc828 255
	#fuel
	poke 0xc828 255
	after time 1 trainer_namcof1racing
}

proc trainer_finalzone {} {
	#power
	poke 0xe024 255
	#ammo
	poke 0xe11c 255
	#grenades
	poke 0xe11b 255
	after time 1 trainer_finalzone
}

proc trainer_zambeze {} {
	#get lives
	poke 0xc025 8
	#get leafs
	poke 0xc011 1
	poke 0xc012 1
	poke 0xc013 1
	poke 0xc014 1
	poke 0xc015 1
	poke 0xc016 1
	poke 0xc017 1
	after time 1 trainer_zambeze
}

proc trainer_pumpkinadventure3 {} {
	#money
	poke 0xd4a8 0x99
	poke 0xd4a9 0x99
	poke 0xd4aa 0x99
	poke 0xd4ab 0x99

	#key card white
	poke 0xd47b 1
	#key
	poke 0xd47c 1
	#crow bar
	poke 0xd47d 1
	#key card green
	poke 0xd47e 1
	#scroll
	poke 0xd47f 1
	#key
	poke 0xd480 1
	#gas can
	poke 0xd481 1
	#marble
	poke 0xd482 1
	#suit
	poke 0xd483 1
	#keycard red
	poke 0xd484 1
	#soup bowl
	poke 0xd485 1
	#necklace
	poke 0xd486 1
	#gloves
	poke 0xd487 1
	#key
	poke 0xd488 1
	#mirror
	poke 0xd489 1
	#cross
	poke 0xd48a 1
	#marble 2
	poke 0xd48b 1
	#key
	poke 0xd48c 1
	#marble 3
	poke 0xd48d 1
	#green opal
	poke 0xd48e 1
	#red opal
	poke 0xd48f 1
	#blue opal
	poke 0xd490 1
	#orange opal
	poke 0xd491 1
	#black opal
	poke 0xd492 1
	#black key
	poke 0xd493 1
	#blue key
	poke 0xd494 1
	#orange key
	poke 0xd495 1
	#torche
	poke 0xd496 1
	#weed key
	poke 0xd497 1
	#butterfly
	poke 0xd498 1
	#maleinsect
	poke 0xd499 1
	#female insect
	poke 0xd49a 1
	#key
	poke 0xd49b 1
	#necklace
	poke 0xd49c 1
	#book
	poke 0xd49d 1
	#marble
	poke 0xd49e 1
	#white keycard 2
	poke 0xd49f 1
	#space suit
	poke 0xd4a0 1
	#mirror 2
	poke 0xd4a1 1
	#cane
	poke 0xd4a2 1
	#red scrolls
	poke 0xd4a3 1
	#gold cross
	poke 0xd4a4 1
	#silver cross
	poke 0xd4a5 1
	#match
	poke 0xd4a6 1
	#wallet
	poke 0xd4a7 1

	#steve life
	poke 0xd5ba 0x99
	poke 0xd5bb 0x99
	#jeff max exp
	poke 0xd5bc 0x99
	poke 0xd5bd 0x99
	#jeff mp
	poke 0xd5be 0x99
	poke 0xd5bf 0x99
	#jeff max mp
	poke 0xd5c0 0x99
	poke 0xd5c1 0x99
	#jeff experience
	poke 0xd5c2 0x99
	poke 0xd5c3 0x99
	poke 0xd5c4 0x99
	#jeff max experience
	poke 0xd5c5 0x00
	poke 0xd5c6 0x00
	poke 0xd5c7 0x01
	#level
	poke 0xd5c8 0x99
	#defence
	poke 0xd5c9 0x99
	poke 0xd5ca 0x99
	#max defense
	poke 0xd5cb 0x99
	poke 0xd5cc 0x99
	#weapon
	poke 0xd5cd 0x9

	#bishop life
	poke 0xd5f3 0x99
	poke 0xd5f4 0x99
	#jeff max exp
	poke 0xd5f5 0x99
	poke 0xd5f6 0x99
	#jeff mp
	poke 0xd5f7 0x99
	poke 0xd5f8 0x99
	#jeff max mp
	poke 0xd5f9 0x99
	poke 0xd5fa 0x99
	#jeff experience
	poke 0xd5fb 0x99
	poke 0xd5fc 0x99
	poke 0xd5fd 0x99
	#jeff max experience
	poke 0xd5fe 0x00
	poke 0xd5ff 0x00
	poke 0xd600 0x01
	#level
	poke 0xd601 0x99
	#defence
	poke 0xd602 0x99
	poke 0xd603 0x99
	#max defense
	poke 0xd604 0x99
	poke 0xd605 0x99
	#weapon
	poke 0xd606 0x9

	#damien life
	poke 0xd62c 0x99
	poke 0xd62d 0x99
	#jeff max exp
	poke 0xd62e 0x99
	poke 0xd62f 0x99
	#jeff mp
	poke 0xd630 0x99
	poke 0xd631 0x99
	#jeff max mp
	poke 0xd632 0x99
	poke 0xd633 0x99
	#jeff experience
	poke 0xd634 0x99
	poke 0xd635 0x99
	poke 0xd636 0x99
	#jeff max experience
	poke 0xd637 0x00
	poke 0xd638 0x00
	poke 0xd639 0x01
	#level
	poke 0xd63a 0x99
	#defence
	poke 0xd63b 0x99
	poke 0xd63c 0x99
	#max defense
	poke 0xd63d 0x99
	poke 0xd63e 0x99
	#weapon
	poke 0xd63f 0x9

	#jeff life
	poke 0xd665 0x99
	poke 0xd666 0x99
	#jeff max exp
	poke 0xd667 0x99
	poke 0xd668 0x99
	#jeff mp
	poke 0xd669 0x99
	poke 0xd66a 0x99
	#jeff max mp
	poke 0xd66b 0x99
	poke 0xd66c 0x99
	#jeff experience
	poke 0xd66d 0x99
	poke 0xd66e 0x99
	poke 0xd66f 0x99
	#jeff max experience
	poke 0xd670 0x00
	poke 0xd671 0x00
	poke 0xd672 0x01
	#level
	poke 0xd673 0x99
	#defence
	poke 0xd674 0x99
	poke 0xd675 0x99
	#max defense
	poke 0xd676 0x99
	poke 0xd677 0x99
	#weapon
	poke 0xd678 0x9

	after time 1 trainer_pumpkinadventure3
}

proc trainer_bluewarrior {} {
	#lives
	poke 0xaacb 8
	#shot (1=normal, 2=fire, 3=electric)
	poke 0xaacd 2
	after time 1 trainer_bluewarrior
}

proc trainer_fairy {} {
	#lives
	poke 0xdf04 5
	after time 5 trainer_fairy
}

proc trainer_mappy {} {
	#lives
	poke 0xe043 11
	after time 5 trainer_mappy
}

proc trainer_warroid {} {
	#power player 1
	poke 0xc58b 255
	#power player 2
	poke 0xc5ab 1
	after time 10 trainer_warroid
}

proc trainer_xyzlogic {} {
	#lives
	poke 0xe418 100
	after time 10 trainer_xyzlogic
}

proc trainer_stepper {} {
	#lives
	poke 0xe901 99
	#have shot
	#!poke 0xe904 1
	after time 5 trainer_stepper
}

proc trainer_exerion1 {} {
	#lives player 1
	poke 0xe108 3
	#lives player 2
	poke 0xe109 3
	#always charge player 1
	poke 0xe120 153
	#always charge player 2
	poke 0xe122 153
	after time 1 trainer_exerion1
}

proc trainer_exerion2 {} {
	#lives player 1
	poke 0xe108 3
	#lives player 2
	poke 0xe109 3
	#always charge player 1
	poke 0xe120 153
	#always charge player 2
	poke 0xe122 153
	after time 1 trainer_exerion2
}

proc trainer_swing {} {
	#lives
	poke 0xe30f 100
	after time 5 trainer_swing
}

proc trainer_david2 {} {
	#lives
	poke 0xe194 255
	after time 5 trainer_david2
}

proc trainer_telebunnie {} {
	#lives
	poke 0xe064 255
	#snake y-position
	poke 0xe018 0
	after time 1 trainer_telebunnie
}

proc trainer_sofia {} {
	#hearts
	poke 0xe6eb 5
	#invincible (except lava and water)
	poke 0xe6ed 255
	after time 5 trainer_sofia
}

proc trainer_boggy84 {} {
	#lives
	poke 0xe693 4
	after time 5 trainer_boggy84
}

proc trainer_riseout {} {
	#lives
	poke 0xec06 255
	#Red man x-postition
	poke 0xed11 255
	#Red man y-postition
	poke 0xed12 191
	after time 5 trainer_riseout
}

proc trainer_dangerx4 {} {
	#lives
	poke 0xd018 10
	#mice left 
	poke 0xd026 1
	after time 1 trainer_dangerx4
}

proc trainer_thefairylandstory {} {
	#lives
	poke 0xe1c0 0x99
	after time 5 trainer_thefairylandstory
}

proc trainer_psychicsoldier2 {} {
	#power
	poke 0xf07a 255
	poke 0xf07b 255
	#fire power
	poke 0xf07e 255
	poke 0xf07f 255
	after time 5 trainer_psychicsoldier2
}

proc trainer_pipi {} {
	#invincible
	poke 0xd623 1
	#invincible timer
	poke 0xd654 255
	#exit always active
	poke 0xd710 2
	#time
	poke 0xd716 20
	#lives
	poke 0xd478 11
	after time 1 trainer_pipi
}

proc trainer_zexaslimited {} {
	#invincible
	poke 0xeecf 100
	after time 60 trainer_zexaslimited
}

proc trainer_bankpanic {} {
	#lives player 1
	poke 0xf587 2
	#extra time player 1
	poke 0xf5b0 119
	#lives player 2
	poke 0xf5bf 2
	#extra time player 2
	poke 0xf5e3 119
	#level (1-9;16-24;...)
	#poke 0xf586 1
	after time 1 trainer_bankpanic
}

proc trainer_bombjackmsx1 {} {
	#lives player 1
	poke 0xc061 4
	#more points player 1
	poke 0xc063 4
	#lives player 2
	poke 0xc084 4
	#more points player 2
	poke 0xc086 4
	#only one frozen enemy
	poke 0xc010 55
	#round (1-50)
	#poke 0xc064 1
	after time 1 trainer_bombjackmsx1
}

proc trainer_bombjackmsx2 {} {
	#lives
	poke 0x483a 52
	#more points
	poke 0x7d2a 3
	#invincible
	poke 0x7d4a 1
	#round (1-80)
	#poke 0x7d20 1
	after time 1 trainer_bombjackmsx2
}

proc trainer_bombjackmsx2promo {} {
	#lives
	poke 0x46b9 52
	#more points
	poke 0x6a89 3
	#invincible
	poke 0x6aa9 1
	#round (1-5)
	#poke 0x6a7f 1
	after time 1 trainer_bombjackmsx2promo
}

proc trainer_chacknpop {} {
	#lives
	poke 0xe06f 3
	#no monsters
	poke 0xe00d 255
	poke 0xe012 255
	poke 0xe017 255
	poke 0xe01c 255
	poke 0xe021 255
	poke 0xe026 255
	poke 0xe02b 255
	poke 0xe030 255
	poke 0xe035 255
	poke 0xe03a 255
	#infinite time
	poke 0xe06b 0
	#maze (1-8)
	#poke 0xe06d 1
	after time 1 trainer_chacknpop
}

proc trainer_choplifter {} {
	#lives
	poke 0xe24d 0
	#only tanks
	poke 0xe27b 0
	after time 1 trainer_choplifter
}

proc trainer_congobongo {} {
	#lives
	poke 0xf34b 3
	#infinite time (to use when the game begins)
	poke 0xf365 0
	after time 1 trainer_congobongo
}

proc trainer_towerofdrauga {} {
	#lives
	poke 0xe9ef 5
	#always have key
	poke 0xea0e 1
	#infinite time
	poke 0xea05 128
	#pick axe
	poke 0xeb64 1
	poke 0xea0d 250
	after time 1 trainer_towerofdrauga
}

proc trainer_rally-x {} {
	#fuel
	poke 0xe039 64
	#lives
	poke 0xe035 5
	after time 1 trainer_rally-x
}

proc trainer_starforce {} {
	#lives
	poke 0xe405 100
	after time 1 trainer_starforce
}

proc trainer_futureknight {} {
	#lives
	poke 0xb4ce 231
	#weapon (25-dart/23 beam/24 fireball)
	poke 0x8075 24
	after time 1 trainer_futureknight
}

proc trainer_astroblaster {} {
	#lives
	poke 0x0575 3
	after time 1 trainer_astroblaster
}

proc trainer_galaxian {} {
	#lives
	poke 0xe071 9
	after time 1 trainer_galaxian
}

proc trainer_donkeykong {} {
	#lives
	poke 0x94ed 9
	after time 1 trainer_donkeykong
}

proc trainer_exterminator {} {
	#lives
	poke 0x4505 58
	after time 1 trainer_exterminator
}

proc trainer_theuntouchables {} {
	#power
	poke 0x70de 255
	after time 1 trainer_theuntouchables
}

proc trainer_barunba {} {
	#energy
	poke 0x6989 6
	#shot (experiment with the value 1-4)
	poke 0x699a 4
	after time 1 trainer_barunba
}

proc trainer_tertisbps {} {
	#next block always bar
	poke 0xd28a 1
	after time 1 trainer_tertisbps
}

proc trainer_magnar {} {
	#live (255 is game over)
	poke 0xca7f 250
	after time 1 trainer_magnar
}

proc trainer_doubledragon2 {} {
	#power player 1
	poke 0x4079 15
	#time
	poke 0x2918 0x99
	after time 1 trainer_doubledragon2
}

proc trainer_cannonball {} {
	#lives
	poke 0xe331 9
	after time 1 trainer_cannonball
}

proc trainer_zenji {} {
	#lives
	poke 0xe1b1 0x99
	after time 1 trainer_zenji
}

proc trainer_legendlyninegems {} {
	#power
	poke 0xb39a 0x99
	poke 0xb399 0x99
	#gold
	poke 0xb39b 0x99
	poke 0xb39c 0x99

	#magic card
	poke 0xb441 255
	#book
	poke 0xb442 255
	#candle
	poke 0xb443 225
	#whool (?)
	poke 0xb444 225
	#drums
	poke 0xb445 255
	#shell
	poke 0xb446 255
	#blue vase
	poke 0xb447 255
	#red vase
	poke 0xb448 255
	#hat
	poke 0xb449 255
	#red botle
	poke 0xb44a 255
	#yellow liquid
	poke 0xb44b 255
	#red liquid
	poke 0xb44c 255
	#key
	poke 0xb44d 3
	after time 1 trainer_legendlyninegems
}

proc trainer_breakoutadventure {} {
	#power
	poke 0x873e 12
	#lives
	poke 0x863d 5
	after time 1 trainer_breakoutadventure
}

proc trainer_vaxol {} {
	#power
	poke 0xc148 255
	after time 1 trainer_vaxol
}

proc trainer_gameover_part1 {} {
	#lives
	poke 0xd9bb 11
	#power
	poke 0xda1b 255
	#second shot
	poke 0xd9bf 100
	after time 1 trainer_gameover_part1
}

proc trainer_gameover_part2 {} {
	#lives
	poke 0xd9bf 12
	#power
	poke 0xda27 255
	#second shot
	poke 0xd9c3 100
	after time 1 trainer_gameover_part2
}

proc trainer_flashsplash  {} {
	#lives -- this trainer might not work 100%
	poke 0xe000 255
	#power
	poke 0xe008 64
	#invincible
	poke 0xe001 0
	poke 0xe009 255
	after time 1 trainer_flashsplash
}

proc trainer_riverraid  {} {
	#fuel
	poke 0xe178 172
	#lives
	poke 0xe135 0x99
	after time 1 trainer_riverraid
}

proc trainer_drainer  {} {
	#lives
	poke 0xec97 99
	#discs
	poke 0xc4c7 3
	after time 1 trainer_drainer
}

proc trainer_moonpatrol  {} {
	#lives
	poke 0xf920 4
	after time 1 trainer_moonpatrol
}

proc trainer_ericandthefloaters  {} {
	#lives
	poke 0xe30f 13
	after time 1 trainer_ericandthefloaters
}

proc trainer_bokosukawars  {} {
	#power (only works when selecting the commander on game start)
	poke 0xd77e 0
	after time 1 trainer_bokosukawars
}

proc trainer_hauntedhouse  {} {
	#lives
	poke 0x4028 6
	#always have keys
	poke 0x747e 2
	#always have shot (f1)
	poke 0x747f 4
	#time
	poke 0x402c 0x9
	poke 0x402d 0x9
	poke 0x402e 0x9
	after time 1 trainer_hauntedhouse
}

proc trainer_ghostbusters2_phase1  {} {
	#ammo
	poke 0x6189 99
	#rope strength
	poke 0x6185 0
	#courage
	poke 0x6180 0
	after time 1 trainer_ghostbusters2_phase1
}

proc trainer_flicky {} {
	#lives
	poke 0xe0ee 3
	#round (1-40)
	#poke 0xe0e7 1
	#poke 0xe0e8 1
	after time 1 trainer_flicky
}

proc trainer_girlsgarden {} {
	#honey (water)
	poke 0xc00e 5
	#love (lives)
	poke 0xc00f 3
	#flowers (very easy !)
	#poke 0xc0be 10
	after time 1 trainer_girlsgarden
}

proc trainer_gulkave {} {
	#energy
	poke 0xe2ad 255
	#weapon (0-16)
	poke 0xe2a7 8
	#lives
	poke 0xe2c5 6
	after time 10 trainer_gulkave
}

proc trainer_hangon {} {
	# time
	poke 0xe04c 100
	# course (0-8)
	#poke 0xe029 0
	# level (0-2)
	#poke 0xe00e 0
	after time 1 trainer_hangon	
}

proc trainer_hustlechumy {} {
	#lives
	poke 0xe153 9
	#infinite time
	poke 0xe1d7 90
	poke 0xe1d9 0
	#level (1-99;0)
	#poke 0xe106 1 0
	after time 1 trainer_hustlechumy
}

proc trainer_3d_bomberman {} {
	#lives
	poke 0xe80f 3
	after time 1 trainer_3d_bomberman
}

proc trainer_demonchristal {} {
	#bombs
	poke 0xf007 9
	poke 0xf008 9
	#lives
	poke 0xf009 9
	poke 0xf00a 9
	#keys
	poke 0xf005 9
	poke 0xf006 9
	#time
	poke 0xf00b 9
	poke 0xf00c 9
	poke 0xf00d 9
	after time 1 trainer_demonchristal
}

proc trainer_negroolipanda {} {
	#power
	poke 0xe2ed 99
	after time 1 trainer_negroolipanda
}

proc trainer_ninjaprincess {} {
	#lives
	poke 0xe047 99
	#partially invincible
	poke 0xe200 0
	poke 0xe343 0
	after time 1 trainer_ninjaprincess
}

proc trainer_hype {} {
	#programmers mode (try B/L/numbers/function keys)
	poke 0xc00d 255
	#invincible
	poke 0xc00a 255
	after time 1 trainer_hype
}

proc trainer_illusioncity {} {
	#money
	poke 0xc268 255
	poke 0xc267 255

	#level Tien Ren
	poke 0xc277 99

	#exp Tien Ren
	poke 0xc275 255
	poke 0xc274 255

	#hp Tien Ren
	poke 0xc282 231
	poke 0xc283 3

	#extended offense tien ren
	poke 0xc288 231
	poke 0xc289 3

	#extended defense Tien Ren
	poke 0xc285 231
	poke 0xc286 3

	#tien extended Ren Agility
	poke 0xc27b 99

	#level Mei Hong
	poke 0xc29f 99

	#exp Mei Hong
	poke 0xc29d 255
	poke 0xc29c 255

	#hp Mei Hong
	poke 0xf636 231
	poke 0xf637 3

	#extended offence mei hong
	poke 0xc2b0 231
	poke 0xc2b1 3

	#extended defende mei hong
	poke 0xc2ad 231
	poke 0xc2ac 3

	#extended agility mei hong
	poke 0xc2a3 99

	#exp old man
	poke 0xc2ed 255
	poke 0xc2ec 255

	#level old man
	poke 0xc2ef 99

	#exp Kash
	poke 0xc33d 255
	poke 0xc33c 255

	#defense
	poke 0xc34d 255

	after time 1 trainer_illusioncity
}

proc trainer_stratos {} {
	#exit always open
	poke 0xe042 0
	#time
	poke 0xe052 99
	#bombs
	poke 0xe053 99
	#hearts
	poke 0xe054 99
	after time 3 trainer_stratos
}

proc trainer_tvirus {} {
	#power
	poke 0xc044 255
	after time 1 trainer_tvirus
}

proc trainer_sasa {} {
	#shots
	poke 0xe07f 255
	#power
	poke 0xe06f 255
	#lives
	poke 0xe005 5
	after time 1 trainer_sasa
}

proc trainer_moonsweeper {} {
	#lives
	poke 0xe136 255
	#invincible
	poke 0xe170 255
	after time 1 trainer_moonsweeper
}

proc trainer_konamissoccer {} {
	#score team 1
	poke 0xe0f5 0x99
	#score team 2
	poke 0xe0f6 0
	after time 1 trainer_konamissoccer
}

proc trainer_konamisgolf {} {
	#shots
	poke 0xe106 1
	after time 1 trainer_konamisgolf
}

proc trainer_blagger {} {
	#lives
	poke 0x9233 0x99
	#air
	poke 0x9a85 255
	after time 1 trainer_blagger
}

proc trainer_chuckieegg {} {
	#lives
	poke 0xb21d 5
	after time 1 trainer_chuckieegg
}

proc trainer_addicataball {} {
	#lives
	poke 0x0616 10
	#ammo
	poke 0x0619 64
	#fuel
	poke 0x0617 64
	#shooting ability
	poke 0x061a 2
	poke 0x061e 1
	#flying ability
	poke 0x618 3
	poke 0x61b 2
	#floor
	poke 0x0aeb 8
	poke 0x0aec 8
	poke 0x0aed 8
	poke 0x0aee 8
	poke 0x0aef 8
	poke 0x0af0 8
	poke 0x0af1 8
	poke 0x0af2 8
	poke 0x0af3 8
	poke 0x0af4 8
	poke 0x0af5 8
	poke 0x0af6 8
	poke 0x0af7 8
	poke 0x0af8 8
	after time 1 trainer_addicataball
}

proc trainer_dogfighter {} {
	#fuel
	poke 0xe33e 255
	poke 0xe33f 255
	#shot
	poke 0xe340 255
	#lives
	poke 0xe304 4
	after time 1 trainer_dogfighter
}

proc trainer_hunchback {} {
	#lives
	poke 0x9114 6
	#y-pos ball 1 
	poke 0x9c72 100
	#y-pos ball 2
	poke 0x9c76 100
	#y-pos wall crawler
	poke 0x9c92 120
	#guardian 1
	poke 0x90ba 10
	#guardian 2
	poke 0x90be 10
	#guardian 3
	poke 0x90c2 10
	#arrow 1
	poke 0x9c6a 100
	#arrow 2
	poke 0x9c6e 100
	after time 0.5 trainer_hunchback
}

proc trainer_anaza {} {
	#power
	poke 0xc016 16
	#speed
	poke 0xc03d 4
	#fire supershot
	poke 0xc03b 6
	#monolis
	poke 0xc020 8
	after time 1 trainer_anaza
}

proc trainer_autofire {} {
	type " "
	after time 0.1 trainer_autofire
}

#alternative names
proc trainer_firehawk {} 	{trainer_thexder2}
proc trainer_nemesis1 {} 	{trainer_gradius1}
proc trainer_nemesis1scc {} 	{trainer_gradius1scc}
proc trainer_nemesis2 {} 	{trainer_gradius2}
proc trainer_nemesis2beta {} 	{trainer_gradius2beta}
proc trainer_nemesis3 {} 	{trainer_gradius3}
proc trainer_BoukenRoman {}	{trainer_dota}
 


### EOF ###