#pragma once
#define CV_ARRAY_SIZE 512
// All CVs are 8-bit numbers (range 0 - 255)
// Note : CV_1 ≙ index 0, CV_2 ≙ index 1, CV_3 ≙ index 2, ...
// TODO: - Packet Timeout

uint8_t CV_ARRAY_DEFAULT [CV_ARRAY_SIZE] = {
   0b00000011,         //CV_1  -    Basic address  CV_1 = 0 is not allowed and used for initiating ADC offset adjustment    //TODO: verify
   0b00001000,         //CV_2  -    V_min   -   Default = 8
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // 0 == Fastest dec/acc rate; 255 == slowest;
   0b00000000,         //CV_3  -    Acceleration rate   -   CV_3*CV_175 = Time for one discrete speed step change in ms
   0b00000000,         //CV_4  -    Deceleration rate   -   CV_4*CV_175 = Time for one discrete speed step change in ms
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b01100100,         //CV_5  -    V_max   -   Default = 100*16
   0b00111111,         //CV_6  -    V_mid   -   Default = 63*16
   0b00000001,         //CV_7  -    Version no.                                                         (read-only)
   0b00001101,         //CV_8  -    Manufacturer (13 = Public Domain & Do-It-Yourself Decoders)         (read-only)
   0b10010110,         //CV_9  -    PWM frequency in Hz = CV_9*100+10000    - Default = (150*100+10000)Hz = 25kHz
   0b00000001,         //CV_10  -
   0b11111111,         //CV_11  -   Packet timeout in seconds
   0b00000100,         //CV_12  -   Permitted operating modes
   0b00000000,         //CV_13  -
   0b00000000,         //CV_14  -
   0b00000000,         //CV_15  -
   0b00000000,         //CV_16  -
   0b11000011,         //CV_17  -   Extended/Long 14-Bit address    (Bits 8 to 13)
   0b11101000,         //CV_18  -   Extended/Long 14-Bit address    (Bits 0 to 7)
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
   0b00010000,         //CV_31  -   Extended CV pointer Bits 8-15
   0b00000000,         //CV_32  -   Extended CV pointer Bits 0-7
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
    // Speed controller - Configuration ////////////////////////////////////////////////////////////////////////////////
   0b00110010,         //CV_47  -   Feed-forward gain k_ff      ≙   CV_47/10000     Default = 50 -> 0.005             //
   0b00001010,         //CV_48  -   PID Control low pass filter time constant (tau) in ms                             //
   0b00000101,         //CV_49  -   PID Control sampling time t in ms                                                 //
   0b00011001,         //CV_50  -   PID Control I_Factor        ≙   CV_50/10        Default = 25 -> 2.5               //
   0b00110010,         //CV_51  -   PID Control D_Factor        ≙   CV_51/10000     Default = 50 -> 0.005             //
   0b01100100,         //CV_52  -   PID Integral Limit positive ≙   CV_52*10        Default = 100 -> +1000            //
   0b01100100,         //CV_53  -   PID Integral Limit negative ≙   CV_53*(-10)     Default = 100 -> -1000            //
   0b00000011,         //CV_54  -   k_p @ x0 - High Byte (Bits 8-15)                divided by 100                    //
   0b11101000,         //CV_55  -   k_p @ x0 - Low Byte (Bits 0-7)                  Default = 1000 -> 10              //
   0b00000000,         //CV_56  -   k_p @ x1 - High Byte (Bits 8-15)                divided by 100                    //
   0b11111010,         //CV_57  -   k_p @ x1 - Low Byte (Bits 0-7)                  Default = 250 -> 2.5              //
   0b00000000,         //CV_58  -   k_p @ x2 - High Byte (Bits 8-15)                divided by 100                    //
   0b10010110,         //CV_59  -   k_p @ x2 - Low Byte (Bits 0-7)                  Default = 150 -> 1.5              //
   0b00001101,         //CV_60  -   x_1 shift in % ≙ CV_60/255                                                        //
   // V_EMF Measurement Configuration //////////////////////////////////////////////////////////////////////////////////
   0b01100100,         //CV_61  -   V_EMF Total amount of Measurement Iterations    -   Default = 100 iterations      //
   0b01100100,         //CV_62  -   V_EMF Delay before Measuring                    -   Default = 100us               //
   // CV_63 and CV_64 are used to determine how many elements of the sorted measurement array will be discarded       //
   // For Example: CV_63 = x and CV_64 = y                                                                            //
   // -> The x smallest values will be discarded.                                                                     //
   // -> The y largest values will be discarded.                                                                      //
   0b00001111,         //CV_63  -   l_side_arr_cutoff      -   Default = 15                                           //
   0b00001111,         //CV_64  -   r_side_arr_cutoff      -   Default = 15                                           //
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b00000000,         //CV_65  -   Values > 0 indicate that flash is still in factory condition
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
///// PWM - Configuration //////////////////////////////////////////////////////////////////////////////////////////////
// This Config can cause conflicts when GPIOs controlled by the same slice are enabled at the same time.
   0b00000000,         //CV_112  -  GPIO 24-31  enable PWM
   0b00000000,         //CV_113  -  GPIO 16-23  enable PWM
   0b00111100,         //CV_114  -  GPIO 8-15   enable PWM
   0b00000000,         //CV_115  -  GPIO 0-7    enable PWM
//Slice 0
   0b00000000,         //CV_116  -  Wrap Bits 8-15
   0b00000000,         //CV_117  -  Wrap Bits 0-7
   0b00000000,         //CV_118  -  Clock Divider
   0b00000000,         //CV_119  -  Channel A Level Bits 8-15
   0b00000000,         //CV_120  -  Channel A Level Bits 0-7
   0b00000000,         //CV_121  -  Channel B Level Bits 8-15
   0b00000000,         //CV_122  -  Channel B Level Bits 0-7
//Slice 1
   0b00000000,         //CV_123  -  Wrap Bits 8-15
   0b00000000,         //CV_124  -  Wrap Bits 0-7
   0b00000000,         //CV_125  -  Clock Divider
   0b00000000,         //CV_126  -  Channel A Level Bits 8-15
   0b00000000,         //CV_127  -  Channel A Level Bits 0-7
   0b00000000,         //CV_128  -  Channel B Level Bits 8-15
   0b00000000,         //CV_129  -  Channel B Level Bits 0-7
//Slice 2
   0b00000000,         //CV_130  -  Wrap Bits 8-15
   0b00000000,         //CV_131  -  Wrap Bits 0-7
   0b00000000,         //CV_132  -  Clock Divider
   0b00000000,         //CV_133  -  Channel A Level Bits 8-15
   0b00000000,         //CV_134  -  Channel A Level Bits 0-7
   0b00000000,         //CV_135  -  Channel B Level Bits 8-15
   0b00000000,         //CV_136  -  Channel B Level Bits 0-7
//Slice 3
   0b00000000,         //CV_137  -  Wrap Bits 8-15
   0b00000000,         //CV_138  -  Wrap Bits 0-7
   0b00000000,         //CV_139  -  Clock Divider
   0b00000000,         //CV_140  -  Channel A Level Bits 8-15
   0b00000000,         //CV_141  -  Channel A Level Bits 0-7
   0b00000000,         //CV_142  -  Channel B Level Bits 8-15
   0b00000000,         //CV_143  -  Channel B Level Bits 0-7
//Slice 4
   0b00000000,         //CV_144  -  Wrap Bits 8-15
   0b00000000,         //CV_145  -  Wrap Bits 0-7
   0b00000000,         //CV_146  -  Clock Divider
   0b00000000,         //CV_147  -  Channel A Level Bits 8-15
   0b00000000,         //CV_148  -  Channel A Level Bits 0-7
   0b00000000,         //CV_149  -  Channel B Level Bits 8-15
   0b00000000,         //CV_150  -  Channel B Level Bits 0-7
//Slice 5
   0b00000000,         //CV_151  -  Wrap Bits 8-15
   0b00000000,         //CV_152  -  Wrap Bits 0-7
   0b00000000,         //CV_153  -  Clock Divider
   0b00000000,         //CV_154  -  Channel A Level Bits 8-15
   0b00000000,         //CV_155  -  Channel A Level Bits 0-7
   0b00000000,         //CV_156  -  Channel B Level Bits 8-15
   0b00000000,         //CV_157  -  Channel B Level Bits 0-7
//Slice 6
   0b00000000,         //CV_158  -  Wrap Bits 8-15
   0b00000000,         //CV_159  -  Wrap Bits 0-7
   0b00000000,         //CV_160  -  Clock Divider
   0b00000000,         //CV_161  -  Channel A Level Bits 8-15
   0b00000000,         //CV_162  -  Channel A Level Bits 0-7
   0b00000000,         //CV_163  -  Channel B Level Bits 8-15
   0b00000000,         //CV_164  -  Channel B Level Bits 0-7
//Slice 7
   0b00000000,         //CV_165  -  Wrap Bits 8-15
   0b00000000,         //CV_166  -  Wrap Bits 0-7
   0b00000000,         //CV_167  -  Clock Divider
   0b00000000,         //CV_168  -  Channel A Level Bits 8-15
   0b00000000,         //CV_169  -  Channel A Level Bits 0-7
   0b00000000,         //CV_170  -  Channel B Level Bits 8-15
   0b00000000,         //CV_171  -  Channel B Level Bits 0-7
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Offset adjustment for ADC                                                                                           //
//If CV_172 = 0xFF = 255 there will be a measurement procedure on startup                                             //
//alternatively, the measurement can be done by writing a 0 to CV_1 on the programming track                          //
   0b11111111,         //CV_172  -  ADC offset                                          Default: 255                 //
   0b01111111,         //CV_173  -  ADC offset measurement cycles                       Default: 127                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b00000001,         //CV_174  -  Additional motor-PWM clock divider.
   0b00000111,         //CV_175  -  speed_helper timer delay -  can be used to adjust accel/decel rate
   0b00000000,         //CV_176  -  Base-PWM forward bits 8-15
   0b00000000,         //CV_177  -  Base-PWM forward bits 0-7
   0b00000000,         //CV_178  -  Base-PWM reverse bits 15-8
   0b00000000,         //CV_179  -  Base-PWM reverse bits 0-7
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
   0b00000000,         //CV_257  -  byte 3
   0b00000000,         //CV_258  -  byte 2
   0b00000000,         //CV_259  -  byte 1
   0b00000000,         //CV_260  -  byte 0
//F0 reverse
   0b00000000,         //CV_261  -  byte 3
   0b00000000,         //CV_262  -  byte 2
   0b00000000,         //CV_263  -  byte 1
   0b00000000,         //CV_264  -  byte 0
//F1 forward
   0b00000000,         //CV_265  -  byte 3
   0b00000000,         //CV_266  -  byte 2
   0b00000000,         //CV_267  -  byte 1
   0b00000000,         //CV_268  -  byte 0
//F1 reverse
   0b00000000,         //CV_269  -  byte 3
   0b00000000,         //CV_270  -  byte 2
   0b00000000,         //CV_271  -  byte 1
   0b00000000,         //CV_272  -  byte 0
//F2 forward
   0b00000000,         //CV_273  -  byte 3
   0b00000000,         //CV_274  -  byte 2
   0b00000000,         //CV_275  -  byte 1
   0b00000000,         //CV_276  -  byte 0
//F2 reverse
   0b00000000,         //CV_277  -  byte 3
   0b00000000,         //CV_278  -  byte 2
   0b00000000,         //CV_279  -  byte 1
   0b00000000,         //CV_280  -  byte 0
//F3 forward
   0b00000000,         //CV_281  -  byte 3
   0b00000000,         //CV_282  -  byte 2
   0b00000000,         //CV_283  -  byte 1
   0b00000000,         //CV_284  -  byte 0
//F3 reverse
   0b00000000,         //CV_285  -  byte 3
   0b00000000,         //CV_286  -  byte 2
   0b00000000,         //CV_287  -  byte 1
   0b00000000,         //CV_288  -  byte 0
//F4 forward
   0b00000000,         //CV_289  -  byte 3
   0b00000000,         //CV_290  -  byte 2
   0b00000000,         //CV_291  -  byte 1
   0b00000000,         //CV_292  -  byte 0
//F4 reverse
   0b00000000,         //CV_293  -  byte 3
   0b00000000,         //CV_294  -  byte 2
   0b00000000,         //CV_295  -  byte 1
   0b00000000,         //CV_296  -  byte 0
//F5 forward
   0b00000000,         //CV_297  -  byte 3
   0b00000000,         //CV_298  -  byte 2
   0b00000000,         //CV_299  -  byte 1
   0b00000000,         //CV_300  -  byte 0
//F5 reverse
   0b00000000,         //CV_301  -  byte 3
   0b00000000,         //CV_302  -  byte 2
   0b00000000,         //CV_303  -  byte 1
   0b00000000,         //CV_304  -  byte 0
//F6 forward
   0b00000000,         //CV_305  -  byte 3
   0b00000000,         //CV_306  -  byte 2
   0b00000000,         //CV_307  -  byte 1
   0b00000000,         //CV_308  -  byte 0
//F6 reverse
   0b00000000,         //CV_309  -  byte 3
   0b00000000,         //CV_310  -  byte 2
   0b00000000,         //CV_311  -  byte 1
   0b00000000,         //CV_312  -  byte 0
//F7 forward
   0b00000000,         //CV_313  -  byte 3
   0b00000000,         //CV_314  -  byte 2
   0b00000000,         //CV_315  -  byte 1
   0b00000000,         //CV_316  -  byte 0
//F7 reverse
   0b00000000,         //CV_317  -  byte 3
   0b00000000,         //CV_318  -  byte 2
   0b00000000,         //CV_319  -  byte 1
   0b00000000,         //CV_320  -  byte 0
//F8 forward
   0b00000000,         //CV_321  -  byte 3
   0b00000000,         //CV_322  -  byte 2
   0b00000000,         //CV_323  -  byte 1
   0b00000000,         //CV_324  -  byte 0
//F8 reverse
   0b00000000,         //CV_325  -  byte 3
   0b00000000,         //CV_326  -  byte 2
   0b00000000,         //CV_327  -  byte 1
   0b00000000,         //CV_328  -  byte 0
//F9 forward
   0b00000000,         //CV_329  -  byte 3
   0b00000000,         //CV_330  -  byte 2
   0b00000000,         //CV_331  -  byte 1
   0b00000000,         //CV_332  -  byte 0
//F9 reverse
   0b00000000,         //CV_333  -  byte 3
   0b00000000,         //CV_334  -  byte 2
   0b00000000,         //CV_335  -  byte 1
   0b00000000,         //CV_336  -  byte 0
//F10 forward
   0b00000000,         //CV_337  -  byte 3
   0b00000000,         //CV_338  -  byte 2
   0b00000000,         //CV_339  -  byte 1
   0b00000000,         //CV_340  -  byte 0
//F10 reverse
   0b00000000,         //CV_341  -  byte 3
   0b00000000,         //CV_342  -  byte 2
   0b00000000,         //CV_343  -  byte 1
   0b00000000,         //CV_344  -  byte 0
//F11 forward
   0b00000000,         //CV_345  -  byte 3
   0b00000000,         //CV_346  -  byte 2
   0b00000000,         //CV_347  -  byte 1
   0b00000000,         //CV_348  -  byte 0
//F11 reverse
   0b00000000,         //CV_349  -  byte 3
   0b00000000,         //CV_350  -  byte 2
   0b00000000,         //CV_351  -  byte 1
   0b00000000,         //CV_352  -  byte 0
//F12 forward
   0b00000000,         //CV_353  -  byte 3
   0b00000000,         //CV_354  -  byte 2
   0b00000000,         //CV_355  -  byte 1
   0b00000000,         //CV_356  -  byte 0
//F12 reverse
   0b00000000,         //CV_357  -  byte 3
   0b00000000,         //CV_358  -  byte 2
   0b00000000,         //CV_359  -  byte 1
   0b00000000,         //CV_360  -  byte 0
//F13 forward
   0b00000000,         //CV_361  -  byte 3
   0b00000000,         //CV_362  -  byte 2
   0b00000000,         //CV_363  -  byte 1
   0b00000000,         //CV_364  -  byte 0
//F13 reverse
   0b00000000,         //CV_365  -  byte 3
   0b00000000,         //CV_366  -  byte 2
   0b00000000,         //CV_367  -  byte 1
   0b00000000,         //CV_368  -  byte 0
//F14 forward
   0b00000000,         //CV_369  -  byte 3
   0b00000000,         //CV_370  -  byte 2
   0b00000000,         //CV_371  -  byte 1
   0b00000000,         //CV_372  -  byte 0
//F14 reverse
   0b00000000,         //CV_373  -  byte 3
   0b00000000,         //CV_374  -  byte 2
   0b00000000,         //CV_375  -  byte 1
   0b00000000,         //CV_376  -  byte 0
//F15 forward
   0b00000000,         //CV_377  -  byte 3
   0b00000000,         //CV_378  -  byte 2
   0b00000000,         //CV_379  -  byte 1
   0b00000000,         //CV_380  -  byte 0
//F15 reverse
   0b00000000,         //CV_381  -  byte 3
   0b00000000,         //CV_382  -  byte 2
   0b00000000,         //CV_383  -  byte 1
   0b00000000,         //CV_384  -  byte 0
//F16 forward
   0b00000000,         //CV_385  -  byte 3
   0b00000000,         //CV_386  -  byte 2
   0b00000000,         //CV_387  -  byte 1
   0b00000000,         //CV_388  -  byte 0
//F16 reverse
   0b00000000,         //CV_389  -  byte 3
   0b00000000,         //CV_390  -  byte 2
   0b00000000,         //CV_391  -  byte 1
   0b00000000,         //CV_392  -  byte 0
//F17 forward
   0b00000000,         //CV_393  -  byte 3
   0b00000000,         //CV_394  -  byte 2
   0b00000000,         //CV_395  -  byte 1
   0b00000000,         //CV_396  -  byte 0
//F17 reverse
   0b00000000,         //CV_397  -  byte 3
   0b00000000,         //CV_398  -  byte 2
   0b00000000,         //CV_399  -  byte 1
   0b00000000,         //CV_400  -  byte 0
//F18 forward
   0b00000000,         //CV_401  -  byte 3
   0b00000000,         //CV_402  -  byte 2
   0b00000000,         //CV_403  -  byte 1
   0b00000000,         //CV_404  -  byte 0
//F18 reverse
   0b00000000,         //CV_405  -  byte 3
   0b00000000,         //CV_406  -  byte 2
   0b00000000,         //CV_407  -  byte 1
   0b00000000,         //CV_408  -  byte 0
//F19 forward
   0b00000000,         //CV_409  -  byte 3
   0b00000000,         //CV_410  -  byte 2
   0b00000000,         //CV_411  -  byte 1
   0b00000000,         //CV_412  -  byte 0
//F19 reverse
   0b00000000,         //CV_413  -  byte 3
   0b00000000,         //CV_414  -  byte 2
   0b00000000,         //CV_415  -  byte 1
   0b00000000,         //CV_416  -  byte 0
//F20 forward
   0b00000000,         //CV_417  -  byte 3
   0b00000000,         //CV_418  -  byte 2
   0b00000000,         //CV_419  -  byte 1
   0b00000000,         //CV_420  -  byte 0
//F20 reverse
   0b00000000,         //CV_421  -  byte 3
   0b00000000,         //CV_422  -  byte 2
   0b00000000,         //CV_423  -  byte 1
   0b00000000,         //CV_424  -  byte 0
//F21 forward
   0b00000000,         //CV_425  -  byte 3
   0b00000000,         //CV_426  -  byte 2
   0b00000000,         //CV_427  -  byte 1
   0b00000000,         //CV_428  -  byte 0
//F21 reverse
   0b00000000,         //CV_429  -  byte 3
   0b00000000,         //CV_430  -  byte 2
   0b00000000,         //CV_431  -  byte 1
   0b00000000,         //CV_432  -  byte 0
//F22 forward
   0b00000000,         //CV_433  -  byte 3
   0b00000000,         //CV_434  -  byte 2
   0b00000000,         //CV_435  -  byte 1
   0b00000000,         //CV_436  -  byte 0
//F22 reverse
   0b00000000,         //CV_437  -  byte 3
   0b00000000,         //CV_438  -  byte 2
   0b00000000,         //CV_439  -  byte 1
   0b00000000,         //CV_440  -  byte 0
//F23 forward
   0b00000000,         //CV_441  -  byte 3
   0b00000000,         //CV_442  -  byte 2
   0b00000000,         //CV_443  -  byte 1
   0b00000000,         //CV_444  -  byte 0
//F23 reverse
   0b00000000,         //CV_445  -  byte 3
   0b00000000,         //CV_446  -  byte 2
   0b00000000,         //CV_447  -  byte 1
   0b00000000,         //CV_448  -  byte 0
//F24 forward
   0b00000000,         //CV_449  -  byte 3
   0b00000000,         //CV_450  -  byte 2
   0b00000000,         //CV_451  -  byte 1
   0b00000000,         //CV_452  -  byte 0
//F24 reverse
   0b00000000,         //CV_453  -  byte 3
   0b00000000,         //CV_454  -  byte 2
   0b00000000,         //CV_455  -  byte 1
   0b00000000,         //CV_456  -  byte 0
//F25 forward
   0b00000000,         //CV_457  -  byte 3
   0b00000000,         //CV_458  -  byte 2
   0b00000000,         //CV_459  -  byte 1
   0b00000000,         //CV_460  -  byte 0
//F25 reverse
   0b00000000,         //CV_461  -  byte 3
   0b00000000,         //CV_462  -  byte 2
   0b00000000,         //CV_463  -  byte 1
   0b00000000,         //CV_464  -  byte 0
//F26 forward
   0b00000000,         //CV_465  -  byte 3
   0b00000000,         //CV_466  -  byte 2
   0b00000000,         //CV_467  -  byte 1
   0b00000000,         //CV_468  -  byte 0
//F26 reverse
   0b00000000,         //CV_469  -  byte 3
   0b00000000,         //CV_470  -  byte 2
   0b00000000,         //CV_471  -  byte 1
   0b00000000,         //CV_472  -  byte 0
//F27 forward
   0b00000000,         //CV_473  -  byte 3
   0b00000000,         //CV_474  -  byte 2
   0b00000000,         //CV_475  -  byte 1
   0b00000000,         //CV_476  -  byte 0
//F27 reverse
   0b00000000,         //CV_477  -  byte 3
   0b00000000,         //CV_478  -  byte 2
   0b00000000,         //CV_479  -  byte 1
   0b00000000,         //CV_480  -  byte 0
//F28 forward
   0b00000000,         //CV_481  -  byte 3
   0b00000000,         //CV_482  -  byte 2
   0b00000000,         //CV_483  -  byte 1
   0b00000000,         //CV_484  -  byte 0
//F28 reverse
   0b00000000,         //CV_485  -  byte 3
   0b00000000,         //CV_486  -  byte 2
   0b00000000,         //CV_487  -  byte 1
   0b00000000,         //CV_488  -  byte 0
//F29 forward
   0b00000000,         //CV_489  -  byte 3
   0b00000000,         //CV_490  -  byte 2
   0b00000000,         //CV_491  -  byte 1
   0b00000000,         //CV_492  -  byte 0
//F29 reverse
   0b00000000,         //CV_493  -  byte 3
   0b00000000,         //CV_494  -  byte 2
   0b00000000,         //CV_495  -  byte 1
   0b00000000,         //CV_496  -  byte 0
//F30 forward
   0b00000000,         //CV_497  -  byte 3
   0b00000000,         //CV_498  -  byte 2
   0b00000000,         //CV_499  -  byte 1
   0b00000000,         //CV_500  -  byte 0
//F30 reverse
   0b00000000,         //CV_501  -  byte 3
   0b00000000,         //CV_502  -  byte 2
   0b00000000,         //CV_503  -  byte 1
   0b00000000,         //CV_504  -  byte 0
//F31 forward
   0b00000000,         //CV_505  -  byte 3
   0b00000000,         //CV_506  -  byte 2
   0b00000000,         //CV_507  -  byte 1
   0b00000000,         //CV_508  -  byte 0
//F31 reverse
   0b00000000,         //CV_509  -  byte 3
   0b00000000,         //CV_510  -  byte 2
   0b00000000,         //CV_511  -  byte 1
   0b00000000,         //CV_512  -  byte 0
};
