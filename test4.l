0000 00100000 ldc 0x1000
0001 0000000b a2sp
0002 ffffff0a adj -1
0003 00004b00 ldc result
0004 00000003 stl 0
0005 00004a00 ldc count
0006 00000004 ldnl 0
0007 00000a0d call main
0008 0000010a adj 1
0009 00000012 HALT
000a fffffd0a main: adj -3
000b 00000103 stl 1
000c 00000203 stl 2
000d 00000000 ldc 0
000e 00000003 stl 0
000f ffffff0a loop: adj -1
0010 00000302 ldl 3
0011 00000003 stl 0
0012 00000102 ldl 1
0013 0000240d call triangle
0014 0000010a adj 1
0015 00000302 ldl 3
0016 00000005 stnl 0
0017 00000302 ldl 3
0018 00000101 adc 1
0019 00000303 stl 3
001a 00000002 ldl 0
001b 00000101 adc 1
001c 00000003 stl 0
001d 00000002 ldl 0
001e 00000202 ldl 2
001f 00000007 sub
0020 ffffee10 brlz loop
0021 00000102 ldl 1
0022 0000030a adj 3
0023 0000000e return
0024 fffffd0a triangle: adj -3
0025 00000103 stl 1
0026 00000203 stl 2
0027 00000100 ldc 1
0028 00000008 shl
0029 00000302 ldl 3
002a 00000007 sub
002b 00000410 brlz skip
002c 00000302 ldl 3
002d 00000202 ldl 2
002e 00000007 sub
002f 00000203 stl 2
0030 00000202 skip: ldl 2
0031 0000140f brz one
0032 00000302 ldl 3
0033 ffffff01 adc -1
0034 00000003 stl 0
0035 ffffff0a adj -1
0036 00000102 ldl 1
0037 00000003 stl 0
0038 00000302 ldl 3
0039 ffffff01 adc -1
003a 0000240d call triangle
003b 00000102 ldl 1
003c 00000003 stl 0
003d 00000103 stl 1
003e 00000302 ldl 3
003f 0000240d call triangle
0040 0000010a adj 1
0041 00000002 ldl 0
0042 00000006 add
0043 00000102 ldl 1
0044 0000030a adj 3
0045 0000000e return
0046 00000100 one: ldc 1
0047 00000102 ldl 1
0048 0000030a adj 3
0049 0000000e return
004a 0000000a count: data 10
004b 00000000 result: data 0