Version 4
SymbolType CELL
LINE Normal -32 -32 32 0
LINE Normal -32 32 32 0
LINE Normal -32 -32 -32 32
LINE Normal -28 -16 -20 -16
LINE Normal -28 16 -20 16
LINE Normal -24 20 -24 12
LINE Normal 0 -32 0 -16
LINE Normal 0 32 0 16
LINE Normal 4 -20 12 -20
LINE Normal 8 -24 8 -16
LINE Normal 4 20 12 20
TEXT 16 32 Left 0 TLC3702
WINDOW 0 16 -32 Left 0
SYMATTR SpiceModel level.2
SYMATTR Prefix X
SYMATTR Description TLC3702 comparator
SYMATTR Value2 Avol=1Meg GBW=2Meg Slew=10Meg
SYMATTR SpiceLine ilimit=20m rail=0.2 Vos=0 phimargin=45
SYMATTR SpiceLine2 en=0 enk=0 in=0 ink=0 Rin=500Meg
SYMATTR ModelFile UniversalOpamps2.sub
PIN -32 16 NONE 0
PINATTR PinName In+
PINATTR SpiceOrder 1
PIN -32 -16 NONE 0
PINATTR PinName In-
PINATTR SpiceOrder 2
PIN 0 -32 NONE 0
PINATTR PinName V+
PINATTR SpiceOrder 3
PIN 0 32 NONE 0
PINATTR PinName V-
PINATTR SpiceOrder 4
PIN 32 0 NONE 0
PINATTR PinName OUT
PINATTR SpiceOrder 5
