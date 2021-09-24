//All CVs are 8-bit numbers (range 0 - 255)
uint8_t CV_ARRAY [512] = {
   0b00000111,         //CV_1  -    Basic Address
   0b00000000,         //CV_2  -    Minimum Speed
   0b00000000,         //CV_3  -    Acceleration Rate
   0b00000000,         //CV_4  -    Deceleration Rate
   0b11111111,         //CV_5  -    V_max
   0b01111111,         //CV_6  -    V_mid
   0b00000001,         //CV_7  -    Version No.
   0b00001101,         //CV_8  -    Manufacturer
   0b00000000,         //CV_9  -    PWM-Period
   0b00000001,         //CV_10  -   PWM Clock Divider
   0b11111111,         //CV_11  -   Packet Timeout in seconds
   0b00000000,         //CV_12  -
   0b00000000,         //CV_13  -
   0b00000000,         //CV_14  -
   0b00000000,         //CV_15  -
   0b00000000,         //CV_16  -
   0b00000000,         //CV_17  -
   0b00000000,         //CV_18  -
   0b00000000,         //CV_19  -
   0b00000000,         //CV_20  -
   0b00000000,         //CV_21  -
   0b00000000,         //CV_22  -
   0b00000000,         //CV_23  -
   0b00000000,         //CV_24  -
   0b00000000,         //CV_25  -
   0b00000000,         //CV_26  -
   0b00000000,         //CV_27  -
   0b00000000,         //CV_28  -
   0b00000000,         //CV_29  -
   0b00000000,         //CV_30  -
   0b00000000,         //CV_31  -
   0b00000000,         //CV_32  -
   0b00000000,         //CV_33  -
   0b00000000,         //CV_34  -
   0b00000000,         //CV_35  -
   0b00000000,         //CV_36  -
   0b00000000,         //CV_37  -
   0b00000000,         //CV_38  -
   0b00000000,         //CV_39  -
   0b00000000,         //CV_40  -
   0b00000000,         //CV_41  -
   0b00000000,         //CV_42  -
   0b00000000,         //CV_43  -
   0b00000000,         //CV_44  -
   0b00000000,         //CV_45  -
   0b00000000,         //CV_46  -
   0b00000000,         //CV_47  -   V_EMF Measure Period duration
   0b00000000,         //CV_48  -   V_EMF Measure Delay
   0b00000000,         //CV_49  -   PID Control P_Factor
   0b00000000,         //CV_50  -   PID Control I_Factor
   0b00000000,         //CV_51  -   PID Control D_Factor
   0b00000000,         //CV_52  -
   0b00000000,         //CV_53  -
   0b00000000,         //CV_54  -
   0b00000000,         //CV_55  -
   0b00000000,         //CV_56  -
   0b00000000,         //CV_57  -
   0b00000000,         //CV_58  -
   0b00000000,         //CV_59  -
   0b00000000,         //CV_60  -
   0b00000000,         //CV_61  -
   0b00000000,         //CV_62  -
   0b00000000,         //CV_63  -
   0b00000000,         //CV_64  -
   0b00000000,         //CV_65  -
   0b00000000,         //CV_66  -
   0b00000000,         //CV_67  -
   0b00000000,         //CV_68  -
   0b00000000,         //CV_69  -
   0b00000000,         //CV_70  -
   0b00000000,         //CV_71  -
   0b00000000,         //CV_72  -
   0b00000000,         //CV_73  -
   0b00000000,         //CV_74  -
   0b00000000,         //CV_75  -
   0b00000000,         //CV_76  -
   0b00000000,         //CV_77  -
   0b00000000,         //CV_78  -
   0b00000000,         //CV_79  -
   0b00000000,         //CV_80  -
   0b00000000,         //CV_81  -
   0b00000000,         //CV_82  -
   0b00000000,         //CV_83  -
   0b00000000,         //CV_84  -
   0b00000000,         //CV_85  -
   0b00000000,         //CV_86  -
   0b00000000,         //CV_87  -
   0b00000000,         //CV_88  -
   0b00000000,         //CV_89  -
   0b00000000,         //CV_90  -
   0b00000000,         //CV_91  -
   0b00000000,         //CV_92  -
   0b00000000,         //CV_93  -
   0b00000000,         //CV_94  -
   0b00000000,         //CV_95  -
   0b00000000,         //CV_96  -
   0b00000000,         //CV_97  -
   0b00000000,         //CV_98  -
   0b00000000,         //CV_99  -
   0b00000000,         //CV_100  -
   0b00000000,         //CV_101  -
   0b00000000,         //CV_102  -
   0b00000000,         //CV_103  -
   0b00000000,         //CV_104  -
   0b00000000,         //CV_105  -
   0b00000000,         //CV_106  -
   0b00000000,         //CV_107  -
   0b00000000,         //CV_108  -
   0b00000000,         //CV_109  -
   0b00000000,         //CV_110  -
   0b00000000,         //CV_111  -
   0b00000000,         //CV_112  -
   0b00000000,         //CV_113  -
   0b00000000,         //CV_114  -
   0b00000000,         //CV_115  -
   0b00000000,         //CV_116  -
   0b00000000,         //CV_117  -
   0b00000000,         //CV_118  -
   0b00000000,         //CV_119  -
   0b00000000,         //CV_120  -
   0b00000000,         //CV_121  -
   0b00000000,         //CV_122  -
   0b00000000,         //CV_123  -
   0b00000000,         //CV_124  -
   0b00000000,         //CV_125  -
   0b00000000,         //CV_126  -
   0b00000000,         //CV_127  -
   0b00000000,         //CV_128  -
   0b00000000,         //CV_129  -
   0b00000000,         //CV_130  -
   0b00000000,         //CV_131  -
   0b00000000,         //CV_132  -
   0b00000000,         //CV_133  -
   0b00000000,         //CV_134  -
   0b00000000,         //CV_135  -
   0b00000000,         //CV_136  -
   0b00000000,         //CV_137  -
   0b00000000,         //CV_138  -
   0b00000000,         //CV_139  -
   0b00000000,         //CV_140  -
   0b00000000,         //CV_141  -
   0b00000000,         //CV_142  -
   0b00000000,         //CV_143  -
   0b00000000,         //CV_144  -
   0b00000000,         //CV_145  -
   0b00000000,         //CV_146  -
   0b00000000,         //CV_147  -
   0b00000000,         //CV_148  -
   0b00000000,         //CV_149  -
   0b00000000,         //CV_150  -
   0b00000000,         //CV_151  -
   0b00000000,         //CV_152  -
   0b00000000,         //CV_153  -
   0b00000000,         //CV_154  -
   0b00000000,         //CV_155  -
   0b00000000,         //CV_156  -
   0b00000000,         //CV_157  -
   0b00000000,         //CV_158  -
   0b00000000,         //CV_159  -
   0b00000000,         //CV_160  -
   0b00000000,         //CV_161  -
   0b00000000,         //CV_162  -
   0b00000000,         //CV_163  -
   0b00000000,         //CV_164  -
   0b00000000,         //CV_165  -
   0b00000000,         //CV_166  -
   0b00000000,         //CV_167  -
   0b00000000,         //CV_168  -
   0b00000000,         //CV_169  -
   0b00000000,         //CV_170  -
   0b00000000,         //CV_171  -
   0b00000000,         //CV_172  -
   0b00000000,         //CV_173  -
   0b00000000,         //CV_174  -
   0b00000000,         //CV_175  -
   0b00000000,         //CV_176  -
   0b00000000,         //CV_177  -
   0b00000000,         //CV_178  -
   0b00000000,         //CV_179  -
   0b00000000,         //CV_180  -
   0b00000000,         //CV_181  -
   0b00000000,         //CV_182  -
   0b00000000,         //CV_183  -
   0b00000000,         //CV_184  -
   0b00000000,         //CV_185  -
   0b00000000,         //CV_186  -
   0b00000000,         //CV_187  -
   0b00000000,         //CV_188  -
   0b00000000,         //CV_189  -
   0b00000000,         //CV_190  -
   0b00000000,         //CV_191  -
   0b00000000,         //CV_192  -
   0b00000000,         //CV_193  -
   0b00000000,         //CV_194  -
   0b00000000,         //CV_195  -
   0b00000000,         //CV_196  -
   0b00000000,         //CV_197  -
   0b00000000,         //CV_198  -
   0b00000000,         //CV_199  -
   0b00000000,         //CV_200  -
   0b00000000,         //CV_201  -
   0b00000000,         //CV_202  -
   0b00000000,         //CV_203  -
   0b00000000,         //CV_204  -
   0b00000000,         //CV_205  -
   0b00000000,         //CV_206  -
   0b00000000,         //CV_207  -
   0b00000000,         //CV_208  -
   0b00000000,         //CV_209  -
   0b00000000,         //CV_210  -
   0b00000000,         //CV_211  -
   0b00000000,         //CV_212  -
   0b00000000,         //CV_213  -
   0b00000000,         //CV_214  -
   0b00000000,         //CV_215  -
   0b00000000,         //CV_216  -
   0b00000000,         //CV_217  -
   0b00000000,         //CV_218  -
   0b00000000,         //CV_219  -
   0b00000000,         //CV_220  -
   0b00000000,         //CV_221  -
   0b00000000,         //CV_222  -
   0b00000000,         //CV_223  -
   0b00000000,         //CV_224  -
   0b00000000,         //CV_225  -
   0b00000000,         //CV_226  -
   0b00000000,         //CV_227  -
   0b00000000,         //CV_228  -
   0b00000000,         //CV_229  -
   0b00000000,         //CV_230  -
   0b00000000,         //CV_231  -
   0b00000000,         //CV_232  -
   0b00000000,         //CV_233  -
   0b00000000,         //CV_234  -
   0b00000000,         //CV_235  -
   0b00000000,         //CV_236  -
   0b00000000,         //CV_237  -
   0b00000000,         //CV_238  -
   0b00000000,         //CV_239  -
   0b00000000,         //CV_240  -
   0b00000000,         //CV_241  -
   0b00000000,         //CV_242  -
   0b00000000,         //CV_243  -
   0b00000000,         //CV_244  -
   0b00000000,         //CV_245  -
   0b00000000,         //CV_246  -
   0b00000000,         //CV_247  -
   0b00000000,         //CV_248  -
   0b00000000,         //CV_249  -
   0b00000000,         //CV_250  -
   0b00000000,         //CV_251  -
   0b00000000,         //CV_252  -
   0b00000000,         //CV_253  -
   0b00000000,         //CV_254  -
   0b00000000,         //CV_255  -
   0b00000000,         //CV_256  -
//FUNCTION MAPPING CVS:
//  31 30 29 28 - 27 26 25 24 -- 23 22 21 20 - 19 18 17 16 -- 15 14 13 12 - 11 10 9  8 -- 7 6 5 4 - 3 2 1 0   <-  GPIO
//           byte_3           --           byte_2          --           byte_1         --       byte_0
//  X  X  X  X  - X  X  X  X  -- X  X  X  X  - X  X  X  X  -- X  X  X  X  - X  X  X  X -- X X X X - X X X X
//F0 forward
    0b00000000,         //byte 0    -   CV_257 - Array Index: 0
    0b00000000,         //byte 1    -   CV_258 - Array Index: 1
    0b00010010,         //byte 2    -   CV_259 - Array Index: 2
    0b00000000,         //byte 3    -   CV_260 - Array Index: 3
//F0 reverse
    0b00000000,         //byte 0    -   CV_261 - Array Index: 4
    0b00000000,         //byte 1    -   CV_262 - Array Index: 5
    0b00001100,         //byte 2    -   CV_263 - Array Index: 6
    0b00000000,         //byte 3    -   CV_264 - Array Index: 7
//F1 forward
    0b00000010,         //byte 0    -   CV_265 - Array Index: 8
    0b00000000,         //byte 1    -   CV_266 - Array Index: 9
    0b00000000,         //byte 2    -   CV_267 - Array Index: 10
    0b00000000,         //byte 3    -   CV_268 - Array Index: 11
//F1 reverse
    0b00000010,         //byte 0    -   CV_269 - Array Index: 12
    0b00000000,         //byte 1    -   CV_270 - Array Index: 13
    0b00000000,         //byte 2    -   CV_271 - Array Index: 14
    0b00000000,         //byte 3    -   CV_272 - Array Index: 15
//F2 forward
    0b00000100,         //byte 0    -   CV_273 - Array Index: 16
    0b00000000,         //byte 1    -   CV_274 - Array Index: 17
    0b00000000,         //byte 2    -   CV_275 - Array Index: 18
    0b00000000,         //byte 3    -   CV_276 - Array Index: 19
//F2 reverse
    0b00000100,         //byte 0    -   CV_277 - Array Index: 20
    0b00000000,         //byte 1    -   CV_278 - Array Index: 21
    0b00000000,         //byte 2    -   CV_279 - Array Index: 22
    0b00000000,         //byte 3    -   CV_280 - Array Index: 23
//F3 forward
    0b00001000,         //byte 0    -   CV_281 - Array Index: 24
    0b00000000,         //byte 1    -   CV_282 - Array Index: 25
    0b00000000,         //byte 2    -   CV_283 - Array Index: 26
    0b00000000,         //byte 3    -   CV_284 - Array Index: 27
//F3 reverse
    0b00001000,         //byte 0    -   CV_285 - Array Index: 28
    0b00000000,         //byte 1    -   CV_286 - Array Index: 29
    0b00000000,         //byte 2    -   CV_287 - Array Index: 30
    0b00000000,         //byte 3    -   CV_288 - Array Index: 31
//F4 forward
    0b00010000,         //byte 0    -   CV_289 - Array Index: 32
    0b00000000,         //byte 1    -   CV_290 - Array Index: 33
    0b00000000,         //byte 2    -   CV_291 - Array Index: 34
    0b00000000,         //byte 3    -   CV_292 - Array Index: 35
//F4 reverse
    0b00010000,         //byte 0    -   CV_293 - Array Index: 36
    0b00000000,         //byte 1    -   CV_294 - Array Index: 37
    0b00000000,         //byte 2    -   CV_295 - Array Index: 38
    0b00000000,         //byte 3    -   CV_296 - Array Index: 39
//F5 forward
    0b00100000,         //byte 0    -   CV_297 - Array Index: 40
    0b00000000,         //byte 1    -   CV_298 - Array Index: 41
    0b00000000,         //byte 2    -   CV_299 - Array Index: 42
    0b00000000,         //byte 3    -   CV_300 - Array Index: 43
//F5 reverse
    0b00100000,         //byte 0    -   CV_301 - Array Index: 44
    0b00000000,         //byte 1    -   CV_302 - Array Index: 45
    0b00000000,         //byte 2    -   CV_303 - Array Index: 46
    0b00000000,         //byte 3    -   CV_304 - Array Index: 47
//F6 forward
    0b01000000,         //byte 0    -   CV_305 - Array Index: 48
    0b00000000,         //byte 1    -   CV_306 - Array Index: 49
    0b00000000,         //byte 2    -   CV_307 - Array Index: 50
    0b00000000,         //byte 3    -   CV_308 - Array Index: 51
//F6 reverse
    0b01000000,         //byte 0    -   CV_309 - Array Index: 52
    0b00000000,         //byte 1    -   CV_310 - Array Index: 53
    0b00000000,         //byte 2    -   CV_311 - Array Index: 54
    0b00000000,         //byte 3    -   CV_312 - Array Index: 55
//F7 forward
    0b10000000,         //byte 0    -   CV_313 - Array Index: 56
    0b00000000,         //byte 1    -   CV_314 - Array Index: 57
    0b00000000,         //byte 2    -   CV_315 - Array Index: 58
    0b00000000,         //byte 3    -   CV_316 - Array Index: 59
//F7 reverse
    0b10000000,         //byte 0    -   CV_317 - Array Index: 60
    0b00000000,         //byte 1    -   CV_318 - Array Index: 61
    0b00000000,         //byte 2    -   CV_319 - Array Index: 62
    0b00000000,         //byte 3    -   CV_320 - Array Index: 63
//F8 forward
    0b00000000,         //byte 0    -   CV_321 - Array Index: 64
    0b00000001,         //byte 1    -   CV_322 - Array Index: 65
    0b00000000,         //byte 2    -   CV_323 - Array Index: 66
    0b00000000,         //byte 3    -   CV_324 - Array Index: 67
//F8 reverse
    0b00000000,         //byte 0    -   CV_325 - Array Index: 68
    0b00000001,         //byte 1    -   CV_326 - Array Index: 69
    0b00000000,         //byte 2    -   CV_327 - Array Index: 70
    0b00000000,         //byte 3    -   CV_328 - Array Index: 71
//F9 forward
    0b00000000,         //byte 0    -   CV_329 - Array Index: 72
    0b00000010,         //byte 1    -   CV_330 - Array Index: 73
    0b00000000,         //byte 2    -   CV_331 - Array Index: 74
    0b00000000,         //byte 3    -   CV_332 - Array Index: 75
//F9 reverse
    0b00000000,         //byte 0    -   CV_333 - Array Index: 76
    0b00000010,         //byte 1    -   CV_334 - Array Index: 77
    0b00000000,         //byte 2    -   CV_335 - Array Index: 78
    0b00000000,         //byte 3    -   CV_336 - Array Index: 79
//F10 forward
    0b00000000,         //byte 0    -   CV_337 - Array Index: 80
    0b00000100,         //byte 1    -   CV_338 - Array Index: 81
    0b00000000,         //byte 2    -   CV_339 - Array Index: 82
    0b00000000,         //byte 3    -   CV_340 - Array Index: 83
//F10 reverse
    0b00000000,         //byte 0    -   CV_341 - Array Index: 84
    0b00000100,         //byte 1    -   CV_342 - Array Index: 85
    0b00000000,         //byte 2    -   CV_343 - Array Index: 86
    0b00000000,         //byte 3    -   CV_344 - Array Index: 87
//F11 forward
    0b00000000,         //byte 0    -   CV_345 - Array Index: 88
    0b00001000,         //byte 1    -   CV_346 - Array Index: 89
    0b00000000,         //byte 2    -   CV_347 - Array Index: 90
    0b00000000,         //byte 3    -   CV_348 - Array Index: 91
//F11 reverse
    0b00000000,         //byte 0    -   CV_349 - Array Index: 92
    0b00001000,         //byte 1    -   CV_350 - Array Index: 93
    0b00000000,         //byte 2    -   CV_351 - Array Index: 94
    0b00000000,         //byte 3    -   CV_352 - Array Index: 95
//F12 forward
    0b00000000,         //byte 0    -   CV_353 - Array Index: 96
    0b00010000,         //byte 1    -   CV_354 - Array Index: 97
    0b00000000,         //byte 2    -   CV_355 - Array Index: 98
    0b00000000,         //byte 3    -   CV_356 - Array Index: 99
//F12 reverse
    0b00000000,         //byte 0    -   CV_357 - Array Index: 100
    0b00010000,         //byte 1    -   CV_358 - Array Index: 101
    0b00000000,         //byte 2    -   CV_359 - Array Index: 102
    0b00000000,         //byte 3    -   CV_360 - Array Index: 103
//F13 forward
    0b00000000,         //byte 0    -   CV_361 - Array Index: 104
    0b00100000,         //byte 1    -   CV_362 - Array Index: 105
    0b00000000,         //byte 2    -   CV_363 - Array Index: 106
    0b00000000,         //byte 3    -   CV_364 - Array Index: 107
//F13 reverse
    0b00000000,         //byte 0    -   CV_365 - Array Index: 108
    0b00100000,         //byte 1    -   CV_366 - Array Index: 109
    0b00000000,         //byte 2    -   CV_367 - Array Index: 110
    0b00000000,         //byte 3    -   CV_368 - Array Index: 111
//F14 forward
    0b00000000,         //byte 0    -   CV_369 - Array Index: 112
    0b01000000,         //byte 1    -   CV_370 - Array Index: 113
    0b00000000,         //byte 2    -   CV_371 - Array Index: 114
    0b00000000,         //byte 3    -   CV_372 - Array Index: 115
//F14 reverse
    0b00000000,         //byte 0    -   CV_373 - Array Index: 116
    0b01000000,         //byte 1    -   CV_374 - Array Index: 117
    0b00000000,         //byte 2    -   CV_375 - Array Index: 118
    0b00000000,         //byte 3    -   CV_376 - Array Index: 119
//F15 forward
    0b00000000,         //byte 0    -   CV_377 - Array Index: 120
    0b10000000,         //byte 1    -   CV_378 - Array Index: 121
    0b00000000,         //byte 2    -   CV_379 - Array Index: 122
    0b00000000,         //byte 3    -   CV_380 - Array Index: 123
//F15 reverse
    0b00000000,         //byte 0    -   CV_381 - Array Index: 124
    0b10000000,         //byte 1    -   CV_382 - Array Index: 125
    0b00000000,         //byte 2    -   CV_383 - Array Index: 126
    0b00000000,         //byte 3    -   CV_384 - Array Index: 127
//F16 forward
    0b00000000,         //byte 0    -   CV_385 - Array Index: 128
    0b00000000,         //byte 1    -   CV_386 - Array Index: 129
    0b00000001,         //byte 2    -   CV_387 - Array Index: 130
    0b00000000,         //byte 3    -   CV_388 - Array Index: 131
//F16 reverse
    0b00000000,         //byte 0    -   CV_389 - Array Index: 132
    0b00000000,         //byte 1    -   CV_390 - Array Index: 133
    0b00000001,         //byte 2    -   CV_391 - Array Index: 134
    0b00000000,         //byte 3    -   CV_392 - Array Index: 135
//F17 forward
    0b00000000,         //byte 0    -   CV_393 - Array Index: 136
    0b00000000,         //byte 1    -   CV_394 - Array Index: 137
    0b00000000,         //byte 2    -   CV_395 - Array Index: 138
    0b00000000,         //byte 3    -   CV_396 - Array Index: 139
//F17 reverse
    0b00000000,         //byte 0    -   CV_397 - Array Index: 140
    0b00000000,         //byte 1    -   CV_398 - Array Index: 141
    0b00000000,         //byte 2    -   CV_399 - Array Index: 142
    0b00000000,         //byte 3    -   CV_400 - Array Index: 143
//F18 forward
    0b00000000,         //byte 0    -   CV_401 - Array Index: 144
    0b00000000,         //byte 1    -   CV_402 - Array Index: 145
    0b00000000,         //byte 2    -   CV_403 - Array Index: 146
    0b00000000,         //byte 3    -   CV_404 - Array Index: 147
//F18 reverse
    0b00000000,         //byte 0    -   CV_405 - Array Index: 148
    0b00000000,         //byte 1    -   CV_406 - Array Index: 149
    0b00000000,         //byte 2    -   CV_407 - Array Index: 150
    0b00000000,         //byte 3    -   CV_408 - Array Index: 151
//F19 forward
    0b00000000,         //byte 0    -   CV_409 - Array Index: 152
    0b00000000,         //byte 1    -   CV_410 - Array Index: 153
    0b00000000,         //byte 2    -   CV_411 - Array Index: 154
    0b00000000,         //byte 3    -   CV_412 - Array Index: 155
//F19 reverse
    0b00000000,         //byte 0    -   CV_413 - Array Index: 156
    0b00000000,         //byte 1    -   CV_414 - Array Index: 157
    0b00000000,         //byte 2    -   CV_415 - Array Index: 158
    0b00000000,         //byte 3    -   CV_416 - Array Index: 159
//F20 forward
    0b00000000,         //byte 0    -   CV_417 - Array Index: 160
    0b00000000,         //byte 1    -   CV_418 - Array Index: 161
    0b00000000,         //byte 2    -   CV_419 - Array Index: 162
    0b00000000,         //byte 3    -   CV_420 - Array Index: 163
//F20 reverse
    0b00000000,         //byte 0    -   CV_421 - Array Index: 164
    0b00000000,         //byte 1    -   CV_422 - Array Index: 165
    0b00000000,         //byte 2    -   CV_423 - Array Index: 166
    0b00000000,         //byte 3    -   CV_424 - Array Index: 167
//F21 forward
    0b00000000,         //byte 0    -   CV_425 - Array Index: 168
    0b00000000,         //byte 1    -   CV_426 - Array Index: 169
    0b00000000,         //byte 2    -   CV_427 - Array Index: 170
    0b00000000,         //byte 3    -   CV_428 - Array Index: 171
//F21 reverse
    0b00000000,         //byte 0    -   CV_429 - Array Index: 172
    0b00000000,         //byte 1    -   CV_430 - Array Index: 173
    0b00000000,         //byte 2    -   CV_431 - Array Index: 174
    0b00000000,         //byte 3    -   CV_432 - Array Index: 175
//F22 forward
    0b00000000,         //byte 0    -   CV_433 - Array Index: 176
    0b00000000,         //byte 1    -   CV_434 - Array Index: 177
    0b00000000,         //byte 2    -   CV_435 - Array Index: 178
    0b00000000,         //byte 3    -   CV_436 - Array Index: 179
//F22 reverse
    0b00000000,         //byte 0    -   CV_437 - Array Index: 180
    0b00000000,         //byte 1    -   CV_438 - Array Index: 181
    0b00000000,         //byte 2    -   CV_439 - Array Index: 182
    0b00000000,         //byte 3    -   CV_440 - Array Index: 183
//F23 forward
    0b00000000,         //byte 0    -   CV_441 - Array Index: 184
    0b00000000,         //byte 1    -   CV_442 - Array Index: 185
    0b00000000,         //byte 2    -   CV_443 - Array Index: 186
    0b00000000,         //byte 3    -   CV_444 - Array Index: 187
//F23 reverse
    0b00000000,         //byte 0    -   CV_445 - Array Index: 188
    0b00000000,         //byte 1    -   CV_446 - Array Index: 189
    0b00000000,         //byte 2    -   CV_447 - Array Index: 190
    0b00000000,         //byte 3    -   CV_448 - Array Index: 191
//F24 forward
    0b00000000,         //byte 0    -   CV_449 - Array Index: 192
    0b00000000,         //byte 1    -   CV_450 - Array Index: 193
    0b00000000,         //byte 2    -   CV_451 - Array Index: 194
    0b00000000,         //byte 3    -   CV_452 - Array Index: 195
//F24 reverse
    0b00000000,         //byte 0    -   CV_453 - Array Index: 196
    0b00000000,         //byte 1    -   CV_454 - Array Index: 197
    0b00000000,         //byte 2    -   CV_455 - Array Index: 198
    0b00000000,         //byte 3    -   CV_456 - Array Index: 199
//F25 forward
    0b00000000,         //byte 0    -   CV_457 - Array Index: 200
    0b00000000,         //byte 1    -   CV_458 - Array Index: 201
    0b00000000,         //byte 2    -   CV_459 - Array Index: 202
    0b00000000,         //byte 3    -   CV_460 - Array Index: 203
//F25 reverse
    0b00000000,         //byte 0    -   CV_461 - Array Index: 204
    0b00000000,         //byte 1    -   CV_462 - Array Index: 205
    0b00000000,         //byte 2    -   CV_463 - Array Index: 206
    0b00000000,         //byte 3    -   CV_464 - Array Index: 207
//F26 forward
    0b00000000,         //byte 0    -   CV_465 - Array Index: 208
    0b00000000,         //byte 1    -   CV_466 - Array Index: 209
    0b00000000,         //byte 2    -   CV_467 - Array Index: 210
    0b00000000,         //byte 3    -   CV_468 - Array Index: 211
//F26 reverse
    0b00000000,         //byte 0    -   CV_469 - Array Index: 212
    0b00000000,         //byte 1    -   CV_470 - Array Index: 213
    0b00000000,         //byte 2    -   CV_471 - Array Index: 214
    0b00000000,         //byte 3    -   CV_472 - Array Index: 215
//F27 forward
    0b00000000,         //byte 0    -   CV_473 - Array Index: 216
    0b00000000,         //byte 1    -   CV_474 - Array Index: 217
    0b00000000,         //byte 2    -   CV_475 - Array Index: 218
    0b00000000,         //byte 3    -   CV_476 - Array Index: 219
//F27 reverse
    0b00000000,         //byte 0    -   CV_477 - Array Index: 220
    0b00000000,         //byte 1    -   CV_478 - Array Index: 221
    0b00000000,         //byte 2    -   CV_479 - Array Index: 222
    0b00000000,         //byte 3    -   CV_480 - Array Index: 223
//F28 forward
    0b00000000,         //byte 0    -   CV_481 - Array Index: 224
    0b00000000,         //byte 1    -   CV_482 - Array Index: 225
    0b00000000,         //byte 2    -   CV_483 - Array Index: 226
    0b00010000,         //byte 3    -   CV_484 - Array Index: 227
//F28 reverse
    0b00000000,         //byte 0    -   CV_485 - Array Index: 228
    0b00000000,         //byte 1    -   CV_486 - Array Index: 229
    0b00000000,         //byte 2    -   CV_487 - Array Index: 230
    0b00010000,         //byte 3    -   CV_488 - Array Index: 231
//F29 forward
    0b00000000,         //byte 0    -   CV_489 - Array Index: 232
    0b00000000,         //byte 1    -   CV_490 - Array Index: 233
    0b00000000,         //byte 2    -   CV_491 - Array Index: 234
    0b00000000,         //byte 3    -   CV_492 - Array Index: 235
//F29 reverse
    0b00000000,         //byte 0    -   CV_493 - Array Index: 236
    0b00000000,         //byte 1    -   CV_494 - Array Index: 237
    0b00000000,         //byte 2    -   CV_495 - Array Index: 238
    0b00000000,         //byte 3    -   CV_496 - Array Index: 239
//F30 forward
    0b00000000,         //byte 0    -   CV_497 - Array Index: 240
    0b00000000,         //byte 1    -   CV_498 - Array Index: 241
    0b00000000,         //byte 2    -   CV_499 - Array Index: 242
    0b00000000,         //byte 3    -   CV_500 - Array Index: 243
//F30 reverse
    0b00000000,         //byte 0    -   CV_501 - Array Index: 244
    0b00000000,         //byte 1    -   CV_502 - Array Index: 245
    0b00000000,         //byte 2    -   CV_503 - Array Index: 246
    0b00000000,         //byte 3    -   CV_504 - Array Index: 247
//F31 forward
    0b00000000,         //byte 0    -   CV_505 - Array Index: 248
    0b00000000,         //byte 1    -   CV_506 - Array Index: 249
    0b00000000,         //byte 2    -   CV_507 - Array Index: 250
    0b00000000,         //byte 3    -   CV_508 - Array Index: 251
//F31 reverse
    0b00000000,         //byte 0    -   CV_509 - Array Index: 252
    0b00000000,         //byte 1    -   CV_510 - Array Index: 253
    0b00000000,         //byte 2    -   CV_511 - Array Index: 254
    0b00000000,         //byte 3    -   CV_512 - Array Index: 255
};
