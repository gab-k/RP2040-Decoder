#include "inttypes.h"
#define CV_ARRAY_SIZE 512
//All CVs are 8-bit numbers (range 0 - 255)
//Note : CV_1 ≙ index 0, CV_2 ≙ index 1, CV_3 ≙ index 2, ...
//TODO:
//      - pwm CV for aux & gpio (duty cycle/freq)
//      - Packet Timeout
//      - Motor PWM Clock & Divider
//      - V_max/V_min & V_emf
//      -
uint8_t CV_ARRAY_DEFAULT [CV_ARRAY_SIZE] = {
   0b00000011,         //CV_1  -    Basic Address
   0b00000000,         //CV_2  -    Minimum Speed
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // 0 == Fastest dec/acc rate; 255 == slowest;
   0b00000000,         //CV_3  -    Acceleration Rate
   0b00000000,         //CV_4  -    Deceleration Rate
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b11111111,         //CV_5  -    V_max
   0b01111111,         //CV_6  -    V_mid
   0b00000001,         //CV_7  -    Version No.
   0b00001101,         //CV_8  -    Manufacturer
   0b00000000,         //CV_9  -    PWM-Period
   0b00000001,         //CV_10  -   PWM Clock Divider
   0b11111111,         //CV_11  -   Packet Timeout in seconds
   0b00000100,         //CV_12  -   Permitted operating modes
   0b00000000,         //CV_13  -
   0b00000000,         //CV_14  -
   0b00000000,         //CV_15  -
   0b00000000,         //CV_16  -
   0b11000011,         //CV_17  -   Extended/Long 14-Bit Address    (Bits 8 to 13)
   0b11101000,         //CV_18  -   Extended/Long 14-Bit Address    (Bits 0 to 7)
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
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Bit_1, Bit_2, Bit_3, Bit_4, Bit_6 are currently not in use and therefore irrelevant.
   //Bit_0 is used to reverse direction i.e. 0 = normal; 1 = reverse
   //Bit_5 switches between basic and extended address i.e. 0 = basic address 1 = extended address
   0b00000000,         //CV_29  -   Decoder Configuration
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
   0b00000000,         //CV_47  -
   // PID - Configuration //////////////////////////////////////////////////////////////////////////////////////////////
   0b00001111,         //CV_48  -   PID Control Sampling Time t_s  in ms ≙ Duration of 1 PID-Cycle                    //
   0b01110011,         //CV_49  -   PID Control P_Factor        ≙   CV_49/256      i.e. Default = 115 -> 0.4492       //
   0b01100110,         //CV_50  -   PID Control I_Factor        ≙   CV_50/64       i.e. Default = 102 -> 1.594 (1/sec)//
   0b01111011,         //CV_51  -   PID Control D_Factor        ≙   CV_51/4096     i.e. Default = 123 -> 0.03002 sec  //
   0b00110000,         //CV_52  -   PID Integral Limit positive ≙ CV_52*10         i.e. Default = 48  -> +480         //
   0b00110000,         //CV_53  -   PID Integral Limit negative ≙ CV_53*(-10)      i.e. Default = 48  -> -480         //
   //This Offset is used to establish a "baseline" motor PWM duty cycle at which the motor starts to move
   //If CV_54+CV_55+CV_56+CV_57 = 0 (after reset or manual overwrite) there will be a measurement procedure on startup//
   //alternatively, the measurement can be done by writing a 7 to CV_7 on the programming track.                      //
   0b00000000,         //CV_54  -   Forward Direction Offset Bits 0-7                   Default = 0                   //
   0b00000000,         //CV_55  -   Forward Direction Offset Bits 8-15                  Default = 0                   //
   0b00000000,         //CV_56  -   Reverse Direction Offset Bits 0-7                   Default = 0                   //
   0b00000000,         //CV_57  -   Reverse Direction Offset Bits 8-15                  Default = 0                   //
   0b00000110,         //CV_58  -   Offset Measurement Cycles                 Default = 6                             //
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b00000111,         //CV_59  -   speed_helper timer delay -  can be used to adjust accel/decel rate                //
   0b00010100,         //CV_60  -   PWM scaling factor (target EMF Voltage) -> 126*CV_60 = end_target|max             //
   // V_EMF Measurement Configuration //////////////////////////////////////////////////////////////////////////////////
   0b01000110,         //CV_61  -   V_EMF Total amount of Measurement Iterations    -   Default = 70 iterations       //
   0b01100100,         //CV_62  -   V_EMF Delay before Measuring                    -   Default = 100us               //
   // CV_63 and CV_64 are used to determine how many elements of the sorted measurement array will be discarded       //
   // This is used to filter out data.                                                                                //
   // For Example: CV_63 = x and CV_64 = y                                                                            //
   // -> The x smallest values will be discarded.                                                                     //
   // -> The y largest values will be discarded.                                                                      //
   0b00001111,         //CV_63  -   left_side_array_cutoff      -   Default = 15                                      //
   0b00001111,         //CV_64  -   right_side_array_cutoff     -   Default = 15                                      //                                                  //
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    0b00000000,         //byte 0    -   CV_257
    0b00001110,         //byte 1    -   CV_258
    0b00000000,         //byte 2    -   CV_259
    0b00000000,         //byte 3    -   CV_260
//F0 reverse
    0b00000000,         //byte 0    -   CV_261
    0b00001110,         //byte 1    -   CV_262
    0b00000000,         //byte 2    -   CV_263
    0b00000000,         //byte 3    -   CV_264
//F1 forward
    0b00000000,         //byte 0    -   CV_265
    0b00000100,         //byte 1    -   CV_266
    0b00000000,         //byte 2    -   CV_267
    0b00000000,         //byte 3    -   CV_268
//F1 reverse
    0b00000000,         //byte 0    -   CV_269
    0b00000010,         //byte 1    -   CV_270
    0b00000000,         //byte 2    -   CV_271
    0b00000000,         //byte 3    -   CV_272
//F2 forward
    0b00000000,         //byte 0    -   CV_273
    0b00000000,         //byte 1    -   CV_274
    0b00000000,         //byte 2    -   CV_275
    0b00000000,         //byte 3    -   CV_276
//F2 reverse
    0b00000000,         //byte 0    -   CV_277
    0b00000000,         //byte 1    -   CV_278
    0b00000000,         //byte 2    -   CV_279
    0b00000000,         //byte 3    -   CV_280
//F3 forward
    0b00000000,         //byte 0    -   CV_281
    0b00000000,         //byte 1    -   CV_282
    0b00000000,         //byte 2    -   CV_283
    0b00000000,         //byte 3    -   CV_284
//F3 reverse
    0b00001000,         //byte 0    -   CV_285
    0b00000000,         //byte 1    -   CV_286
    0b00000000,         //byte 2    -   CV_287
    0b00000000,         //byte 3    -   CV_288
//F4 forward
    0b00010000,         //byte 0    -   CV_289
    0b00000000,         //byte 1    -   CV_290
    0b00000000,         //byte 2    -   CV_291
    0b00000000,         //byte 3    -   CV_292
//F4 reverse
    0b00010000,         //byte 0    -   CV_293
    0b00000000,         //byte 1    -   CV_294
    0b00000000,         //byte 2    -   CV_295
    0b00000000,         //byte 3    -   CV_296
//F5 forward
    0b00100000,         //byte 0    -   CV_297
    0b00000000,         //byte 1    -   CV_298
    0b00000000,         //byte 2    -   CV_299
    0b00000000,         //byte 3    -   CV_300
//F5 reverse
    0b00100000,         //byte 0    -   CV_301
    0b00000000,         //byte 1    -   CV_302
    0b00000000,         //byte 2    -   CV_303
    0b00000000,         //byte 3    -   CV_304
//F6 forward
    0b01000000,         //byte 0    -   CV_305
    0b00000000,         //byte 1    -   CV_306
    0b00000000,         //byte 2    -   CV_307
    0b00000000,         //byte 3    -   CV_308
//F6 reverse
    0b01000000,         //byte 0    -   CV_309
    0b00000000,         //byte 1    -   CV_310
    0b00000000,         //byte 2    -   CV_311
    0b00000000,         //byte 3    -   CV_312
//F7 forward
    0b10000000,         //byte 0    -   CV_313
    0b00000000,         //byte 1    -   CV_314
    0b00000000,         //byte 2    -   CV_315
    0b00000000,         //byte 3    -   CV_316
//F7 reverse
    0b10000000,         //byte 0    -   CV_317
    0b00000000,         //byte 1    -   CV_318
    0b00000000,         //byte 2    -   CV_319
    0b00000000,         //byte 3    -   CV_320
//F8 forward
    0b00000000,         //byte 0    -   CV_321
    0b00000001,         //byte 1    -   CV_322
    0b00000000,         //byte 2    -   CV_323
    0b00000000,         //byte 3    -   CV_324
//F8 reverse
    0b00000000,         //byte 0    -   CV_325
    0b00000001,         //byte 1    -   CV_326
    0b00000000,         //byte 2    -   CV_327
    0b00000000,         //byte 3    -   CV_328
//F9 forward
    0b00000000,         //byte 0    -   CV_329
    0b00000010,         //byte 1    -   CV_330
    0b00000000,         //byte 2    -   CV_331
    0b00000000,         //byte 3    -   CV_332
//F9 reverse
    0b00000000,         //byte 0    -   CV_333
    0b00000010,         //byte 1    -   CV_334
    0b00000000,         //byte 2    -   CV_335
    0b00000000,         //byte 3    -   CV_336
//F10 forward
    0b00000000,         //byte 0    -   CV_337
    0b00000100,         //byte 1    -   CV_338
    0b00000000,         //byte 2    -   CV_339
    0b00000000,         //byte 3    -   CV_340
//F10 reverse
    0b00000000,         //byte 0    -   CV_341
    0b00000100,         //byte 1    -   CV_342
    0b00000000,         //byte 2    -   CV_343
    0b00000000,         //byte 3    -   CV_344
//F11 forward
    0b00000000,         //byte 0    -   CV_345
    0b00001000,         //byte 1    -   CV_346
    0b00000000,         //byte 2    -   CV_347
    0b00000000,         //byte 3    -   CV_348
//F11 reverse
    0b00000000,         //byte 0    -   CV_349
    0b00001000,         //byte 1    -   CV_350
    0b00000000,         //byte 2    -   CV_351
    0b00000000,         //byte 3    -   CV_352
//F12 forward
    0b00000000,         //byte 0    -   CV_353
    0b00010000,         //byte 1    -   CV_354
    0b00000000,         //byte 2    -   CV_355
    0b00000000,         //byte 3    -   CV_356
//F12 reverse
    0b00000000,         //byte 0    -   CV_357
    0b00010000,         //byte 1    -   CV_358
    0b00000000,         //byte 2    -   CV_359
    0b00000000,         //byte 3    -   CV_360
//F13 forward
    0b00000000,         //byte 0    -   CV_361
    0b00100000,         //byte 1    -   CV_362
    0b00000000,         //byte 2    -   CV_363
    0b00000000,         //byte 3    -   CV_364
//F13 reverse
    0b00000000,         //byte 0    -   CV_365
    0b00100000,         //byte 1    -   CV_366
    0b00000000,         //byte 2    -   CV_367
    0b00000000,         //byte 3    -   CV_368
//F14 forward
    0b00000000,         //byte 0    -   CV_369
    0b01000000,         //byte 1    -   CV_370
    0b00000000,         //byte 2    -   CV_371
    0b00000000,         //byte 3    -   CV_372
//F14 reverse
    0b00000000,         //byte 0    -   CV_373
    0b01000000,         //byte 1    -   CV_374
    0b00000000,         //byte 2    -   CV_375
    0b00000000,         //byte 3    -   CV_376
//F15 forward
    0b00000000,         //byte 0    -   CV_377
    0b10000000,         //byte 1    -   CV_378
    0b00000000,         //byte 2    -   CV_379
    0b00000000,         //byte 3    -   CV_380
//F15 reverse
    0b00000000,         //byte 0    -   CV_381
    0b10000000,         //byte 1    -   CV_382
    0b00000000,         //byte 2    -   CV_383
    0b00000000,         //byte 3    -   CV_384
//F16 forward
    0b00000000,         //byte 0    -   CV_385
    0b00000000,         //byte 1    -   CV_386
    0b00000001,         //byte 2    -   CV_387
    0b00000000,         //byte 3    -   CV_388
//F16 reverse
    0b00000000,         //byte 0    -   CV_389
    0b00000000,         //byte 1    -   CV_390
    0b00000001,         //byte 2    -   CV_391
    0b00000000,         //byte 3    -   CV_392
//F17 forward
    0b00000000,         //byte 0    -   CV_393
    0b00000000,         //byte 1    -   CV_394
    0b00000000,         //byte 2    -   CV_395
    0b00000000,         //byte 3    -   CV_396
//F17 reverse
    0b00000000,         //byte 0    -   CV_397
    0b00000000,         //byte 1    -   CV_398
    0b00000000,         //byte 2    -   CV_399
    0b00000000,         //byte 3    -   CV_400
//F18 forward
    0b00000000,         //byte 0    -   CV_401
    0b00000000,         //byte 1    -   CV_402
    0b00000000,         //byte 2    -   CV_403
    0b00000000,         //byte 3    -   CV_404
//F18 reverse
    0b00000000,         //byte 0    -   CV_405
    0b00000000,         //byte 1    -   CV_406
    0b00000000,         //byte 2    -   CV_407
    0b00000000,         //byte 3    -   CV_408
//F19 forward
    0b00000000,         //byte 0    -   CV_409
    0b00000000,         //byte 1    -   CV_410
    0b00000000,         //byte 2    -   CV_411
    0b00000000,         //byte 3    -   CV_412
//F19 reverse
    0b00000000,         //byte 0    -   CV_413
    0b00000000,         //byte 1    -   CV_414
    0b00000000,         //byte 2    -   CV_415
    0b00000000,         //byte 3    -   CV_416
//F20 forward
    0b00000000,         //byte 0    -   CV_417
    0b00000000,         //byte 1    -   CV_418
    0b00000000,         //byte 2    -   CV_419
    0b00000000,         //byte 3    -   CV_420
//F20 reverse
    0b00000000,         //byte 0    -   CV_421
    0b00000000,         //byte 1    -   CV_422
    0b00000000,         //byte 2    -   CV_423
    0b00000000,         //byte 3    -   CV_424
//F21 forward
    0b00000000,         //byte 0    -   CV_425
    0b00000000,         //byte 1    -   CV_426
    0b00000000,         //byte 2    -   CV_427
    0b00000000,         //byte 3    -   CV_428
//F21 reverse
    0b00000000,         //byte 0    -   CV_429
    0b00000000,         //byte 1    -   CV_430
    0b00000000,         //byte 2    -   CV_431
    0b00000000,         //byte 3    -   CV_432
//F22 forward
    0b00000000,         //byte 0    -   CV_433
    0b00000000,         //byte 1    -   CV_434
    0b00000000,         //byte 2    -   CV_435
    0b00000000,         //byte 3    -   CV_436
//F22 reverse
    0b00000000,         //byte 0    -   CV_437
    0b00000000,         //byte 1    -   CV_438
    0b00000000,         //byte 2    -   CV_439
    0b00000000,         //byte 3    -   CV_440
//F23 forward
    0b00000000,         //byte 0    -   CV_441
    0b00000000,         //byte 1    -   CV_442
    0b00000000,         //byte 2    -   CV_443
    0b00000000,         //byte 3    -   CV_444
//F23 reverse
    0b00000000,         //byte 0    -   CV_445
    0b00000000,         //byte 1    -   CV_446
    0b00000000,         //byte 2    -   CV_447
    0b00000000,         //byte 3    -   CV_448
//F24 forward
    0b00000000,         //byte 0    -   CV_449
    0b00000000,         //byte 1    -   CV_450
    0b00000000,         //byte 2    -   CV_451
    0b00000000,         //byte 3    -   CV_452
//F24 reverse
    0b00000000,         //byte 0    -   CV_453
    0b00000000,         //byte 1    -   CV_454
    0b00000000,         //byte 2    -   CV_455
    0b00000000,         //byte 3    -   CV_456
//F25 forward
    0b00000000,         //byte 0    -   CV_457
    0b00000000,         //byte 1    -   CV_458
    0b00000000,         //byte 2    -   CV_459
    0b00000000,         //byte 3    -   CV_460
//F25 reverse
    0b00000000,         //byte 0    -   CV_461
    0b00000000,         //byte 1    -   CV_462
    0b00000000,         //byte 2    -   CV_463
    0b00000000,         //byte 3    -   CV_464
//F26 forward
    0b00000000,         //byte 0    -   CV_465
    0b00000000,         //byte 1    -   CV_466
    0b00000000,         //byte 2    -   CV_467
    0b00000000,         //byte 3    -   CV_468
//F26 reverse
    0b00000000,         //byte 0    -   CV_469
    0b00000000,         //byte 1    -   CV_470
    0b00000000,         //byte 2    -   CV_471
    0b00000000,         //byte 3    -   CV_472
//F27 forward
    0b00000000,         //byte 0    -   CV_473
    0b00000000,         //byte 1    -   CV_474
    0b00000000,         //byte 2    -   CV_475
    0b00000000,         //byte 3    -   CV_476
//F27 reverse
    0b00000000,         //byte 0    -   CV_477
    0b00000000,         //byte 1    -   CV_478
    0b00000000,         //byte 2    -   CV_479
    0b00000000,         //byte 3    -   CV_480
//F28 forward
    0b11111100,         //byte 0    -   CV_481
    0b00001111,         //byte 1    -   CV_482
    0b00000000,         //byte 2    -   CV_483
    0b00000000,         //byte 3    -   CV_484
//F28 reverse
    0b11111100,         //byte 0    -   CV_485
    0b00001111,         //byte 1    -   CV_486
    0b00000000,         //byte 2    -   CV_487
    0b00000000,         //byte 3    -   CV_488
//F29 forward
    0b00000000,         //byte 0    -   CV_489
    0b00000000,         //byte 1    -   CV_490
    0b00000000,         //byte 2    -   CV_491
    0b00000000,         //byte 3    -   CV_492
//F29 reverse
    0b00000000,         //byte 0    -   CV_493
    0b00000000,         //byte 1    -   CV_494
    0b00000000,         //byte 2    -   CV_495
    0b00000000,         //byte 3    -   CV_496
//F30 forward
    0b00000000,         //byte 0    -   CV_497
    0b00000000,         //byte 1    -   CV_498
    0b00000000,         //byte 2    -   CV_499
    0b00000000,         //byte 3    -   CV_500
//F30 reverse
    0b00000000,         //byte 0    -   CV_501
    0b00000000,         //byte 1    -   CV_502
    0b00000000,         //byte 2    -   CV_503
    0b00000000,         //byte 3    -   CV_504
//F31 forward
    0b00000000,         //byte 0    -   CV_505
    0b00000000,         //byte 1    -   CV_506
    0b00000000,         //byte 2    -   CV_507
    0b00000000,         //byte 3    -   CV_508
//F31 reverse
    0b00000000,         //byte 0    -   CV_509
    0b00000000,         //byte 1    -   CV_510
    0b00000000,         //byte 2    -   CV_511
    0b00000000,         //byte 3    -   CV_512
};
