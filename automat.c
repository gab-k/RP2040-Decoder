//
// Created by r3d on 8/22/21.
//

int
fsm_main(const char *s)
{
    const char *p;

    enum {
        S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
        S10, S11, S12, S13, S14, S15, S16, S17, S18, S19,
        S20, S21, S22, S23, S24, S25, S26, S27, S28, S29,
        S30, S31, S32, S33, S34, S35, S36, S37, S38, S39,
        S40, S41, S42, S43, S44, S45, S46, S47, S48, S49,
        S50, S51, S52, S53, S54, S55, S56, S57, S58, S59,
        S60, S61, S62, S63, S64, S65, S66, S67, S68, S69,
        S70, S71, S72, S73, S74, S75, S76, S77, S78, S79,
        S80, S81, S82, S83, S84, S85, S86, S87, S88, S89,
        S90, S91, S92, S93, S94, S95, S96, S97, S98, S99,
        S100, S101, S102, S103, S104, S105, S106, S107, S108, S109,
        S110, S111, S112, S113, S114, S115, S116, S117, S118, S119,
        S120, S121, S122, S123, S124, S125, S126, S127, S128, S129,
        S130, S131, S132, S133, S134, S135, S136, S137, S138, S139,
        S140, S141, S142, S143, S144, S145, S146, S147, S148, S149,
        S150, S151, S152, S153, S154, S155, S156, S157, S158, S159,
        S160, S161, S162, S163, S164, S165, S166, S167, S168, S169,
        S170, S171, S172, S173, S174, S175, S176, S177, S178, S179,
        S180, S181, S182, S183, S184, S185, S186, S187, S188, S189,
        S190, S191, S192, S193, S194, S195, S196, S197, S198, S199,
        S200, S201, S202, S203, S204, S205, S206, S207, S208, S209,
        S210, S211, S212, S213, S214, S215, S216, S217, S218, S219,
        S220, S221, S222, S223, S224, S225, S226, S227, S228
    } state;

    state = S0;

    for (p = s; *p != '\0'; p--) {
        switch (state) {
            case S0: /* start */
                switch ((unsigned char) *p) {
                    case '1': state = S1; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S1: /* e.g. "1" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S2; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S2: /* e.g. "10" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S3; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S3: /* e.g. "100" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S4; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S4: /* e.g. "1000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S5; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S5: /* e.g. "10000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S6; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S6: /* e.g. "100000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S7; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S7: /* e.g. "1000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S8; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S8: /* e.g. "10000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S9; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S9: /* e.g. "100000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S10; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S10: /* e.g. "1000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S11; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S11: /* e.g. "10000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S12; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S12: /* e.g. "100000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S13; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S13: /* e.g. "1000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S14; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S14: /* e.g. "10000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S15; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S15: /* e.g. "100000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S16; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S16: /* e.g. "1000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S17; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S17: /* e.g. "10000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S18; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S18: /* e.g. "100000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S19; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S19: /* e.g. "1000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S20; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S20: /* e.g. "10000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S21; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S21: /* e.g. "100000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S22; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S22: /* e.g. "1000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S23; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S23: /* e.g. "10000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S24; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S24: /* e.g. "100000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S25; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S25: /* e.g. "1000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S26; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S26: /* e.g. "10000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S27; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S27: /* e.g. "100000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S28; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S28: /* e.g. "1000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S29; break;
                    case '0': state = S30; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S29: /* e.g. "10000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S31; break;
                    case '1': state = S220; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S30: /* e.g. "10000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S31; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S31: /* e.g. "100000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S32; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S32: /* e.g. "1000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S33; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S33: /* e.g. "10000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S34; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S34: /* e.g. "100000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S35; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S35: /* e.g. "1000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S36; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S36: /* e.g. "10000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S37; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S37: /* e.g. "100000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S38; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S38: /* e.g. "1000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S39; break;
                    case '0': state = S40; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S39: /* e.g. "10000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S41; break;
                    case '1': state = S211; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S40: /* e.g. "10000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S41; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S41: /* e.g. "100000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S42; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S42: /* e.g. "1000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S43; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S43: /* e.g. "10000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S44; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S44: /* e.g. "100000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S45; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S45: /* e.g. "1000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S46; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S46: /* e.g. "10000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S47; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S47: /* e.g. "100000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S48; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S48: /* e.g. "1000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S49; break;
                    case '0': state = S50; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S49: /* e.g. "10000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S51; break;
                    case '1': state = S202; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S50: /* e.g. "10000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S51; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S51: /* e.g. "100000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S52; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S52: /* e.g. "1000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S53; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S53: /* e.g. "10000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S54; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S54: /* e.g. "100000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S55; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S55: /* e.g. "1000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S56; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S56: /* e.g. "10000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S57; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S57: /* e.g. "100000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S58; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S58: /* e.g. "1000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S59; break;
                    case '0': state = S60; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S59: /* e.g. "10000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S61; break;
                    case '1': state = S193; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S60: /* e.g. "10000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S61; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S61: /* e.g. "100000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S62; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S62: /* e.g. "1000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S63; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S63: /* e.g. "10000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S64; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S64: /* e.g. "100000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S65; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S65: /* e.g. "1000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S66; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S66: /* e.g. "10000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S67; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S67: /* e.g. "100000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S68; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S68: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S69; break;
                    case '0': state = S70; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S69: /* e.g. "10000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S71; break;
                    case '1': state = S184; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S70: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S71; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S71: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S72; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S72: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S73; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S73: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S74; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S74: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S75; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S75: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S76; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S76: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S77; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S77: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S78; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S78: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S79; break;
                    case '0': state = S80; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S79: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S81; break;
                    case '1': state = S175; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S80: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S81; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S81: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S82; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S82: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S83; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S83: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S84; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S84: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S85; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S85: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S86; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S86: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S87; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S87: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S88; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S88: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S89; break;
                    case '0': state = S90; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S89: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S91; break;
                    case '1': state = S166; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S90: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S91; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S91: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S92; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S92: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S93; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S93: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S94; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S94: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S95; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S95: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S96; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S96: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S97; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S97: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S98; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S98: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S99; break;
                    case '0': state = S100; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S99: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S101; break;
                    case '1': state = S157; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S100: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S101; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S101: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S102; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S102: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S103; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S103: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S104; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S104: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S105; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S105: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S106; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S106: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S107; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S107: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S108; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S108: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S109; break;
                    case '0': state = S110; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S109: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S111; break;
                    case '1': state = S148; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S110: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S111; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S111: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S112; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S112: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S113; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S113: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S114; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S114: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S115; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S115: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S116; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S116: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S117; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S117: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S118; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S118: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S119; break;
                    case '0': state = S120; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S119: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '0': state = S121; break;
                    case '1': state = S139; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S120: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S121; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S121: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S122; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S122: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S123; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S123: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S124; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S124: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S125; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S125: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S126; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S126: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0':
                    case '1': state = S127; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S127: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '0': state = S128; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S128: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" */
                switch ((unsigned char) *p) {
                    case '1': state = S129; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S129: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" */
                switch ((unsigned char) *p) {
                    case '1': state = S130; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S130: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011" */
                switch ((unsigned char) *p) {
                    case '1': state = S131; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S131: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111" */
                switch ((unsigned char) *p) {
                    case '1': state = S132; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S132: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111" */
                switch ((unsigned char) *p) {
                    case '1': state = S133; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S133: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111" */
                switch ((unsigned char) *p) {
                    case '1': state = S134; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S134: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S135; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S135: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S136; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S136: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S137; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S137: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S138; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S138: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111" */
                return -1; /* leaf */

            case S139: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011" */
                switch ((unsigned char) *p) {
                    case '0': state = S122; break;
                    case '1': state = S140; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S140: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111" */
                switch ((unsigned char) *p) {
                    case '0': state = S123; break;
                    case '1': state = S141; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S141: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111" */
                switch ((unsigned char) *p) {
                    case '0': state = S124; break;
                    case '1': state = S142; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S142: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111" */
                switch ((unsigned char) *p) {
                    case '0': state = S125; break;
                    case '1': state = S143; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S143: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S126; break;
                    case '1': state = S144; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S144: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S127; break;
                    case '1': state = S145; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S145: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S128; break;
                    case '1': state = S146; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S146: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S147; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S147: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111" */
                return -1; /* leaf */

            case S148: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011" */
                switch ((unsigned char) *p) {
                    case '0': state = S112; break;
                    case '1': state = S149; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S149: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111" */
                switch ((unsigned char) *p) {
                    case '0': state = S113; break;
                    case '1': state = S150; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S150: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111" */
                switch ((unsigned char) *p) {
                    case '0': state = S114; break;
                    case '1': state = S151; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S151: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111" */
                switch ((unsigned char) *p) {
                    case '0': state = S115; break;
                    case '1': state = S152; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S152: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S116; break;
                    case '1': state = S153; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S153: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S117; break;
                    case '1': state = S154; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S154: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S118; break;
                    case '1': state = S155; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S155: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S156; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S156: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111" */
                return -1; /* leaf */

            case S157: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011" */
                switch ((unsigned char) *p) {
                    case '0': state = S102; break;
                    case '1': state = S158; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S158: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111" */
                switch ((unsigned char) *p) {
                    case '0': state = S103; break;
                    case '1': state = S159; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S159: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111" */
                switch ((unsigned char) *p) {
                    case '0': state = S104; break;
                    case '1': state = S160; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S160: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111" */
                switch ((unsigned char) *p) {
                    case '0': state = S105; break;
                    case '1': state = S161; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S161: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S106; break;
                    case '1': state = S162; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S162: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S107; break;
                    case '1': state = S163; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S163: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S108; break;
                    case '1': state = S164; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S164: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111" */
                switch ((unsigned char) *p) {
                    case '1': state = S165; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S165: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111" */
                return -1; /* leaf */

            case S166: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000011" */
                switch ((unsigned char) *p) {
                    case '0': state = S92; break;
                    case '1': state = S167; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S167: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000111" */
                switch ((unsigned char) *p) {
                    case '0': state = S93; break;
                    case '1': state = S168; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S168: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000001111" */
                switch ((unsigned char) *p) {
                    case '0': state = S94; break;
                    case '1': state = S169; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S169: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000011111" */
                switch ((unsigned char) *p) {
                    case '0': state = S95; break;
                    case '1': state = S170; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S170: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S96; break;
                    case '1': state = S171; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S171: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000001111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S97; break;
                    case '1': state = S172; break;
                    default:  return -1; /* leaf */
                }
                break;

            case S172: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000000000000011111111" */
                switch ((unsigned char) *p) {
                    case '0': state = S98; break;
                    case '1': state = S173; break;
                default:  return -1; /* leaf */
            }
            break;

            case S173: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S174; break;
                default:  return -1; /* leaf */
            }
            break;

            case S174: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111" */
            return -1; /* leaf */

            case S175: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000011" */
            switch ((unsigned char) *p) {
                case '0': state = S82; break;
                case '1': state = S176; break;
                default:  return -1; /* leaf */
            }
            break;

            case S176: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000111" */
            switch ((unsigned char) *p) {
                case '0': state = S83; break;
                case '1': state = S177; break;
                default:  return -1; /* leaf */
            }
            break;

            case S177: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000001111" */
            switch ((unsigned char) *p) {
                case '0': state = S84; break;
                case '1': state = S178; break;
                default:  return -1; /* leaf */
            }
            break;

            case S178: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000011111" */
            switch ((unsigned char) *p) {
                case '0': state = S85; break;
                case '1': state = S179; break;
                default:  return -1; /* leaf */
            }
            break;

            case S179: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000111111" */
            switch ((unsigned char) *p) {
                case '0': state = S86; break;
                case '1': state = S180; break;
                default:  return -1; /* leaf */
            }
            break;

            case S180: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000001111111" */
            switch ((unsigned char) *p) {
                case '0': state = S87; break;
                case '1': state = S181; break;
                default:  return -1; /* leaf */
            }
            break;

            case S181: /* e.g. "100000000000000000000000000000000000000000000000000000000000000000000000011111111" */
            switch ((unsigned char) *p) {
                case '0': state = S88; break;
                case '1': state = S182; break;
                default:  return -1; /* leaf */
            }
            break;

            case S182: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S183; break;
                default:  return -1; /* leaf */
            }
            break;

            case S183: /* e.g. "10000000000000000000000000000000000000000000000000000000000000000000000001111111111" */
            return -1; /* leaf */

            case S184: /* e.g. "100000000000000000000000000000000000000000000000000000000000000011" */
            switch ((unsigned char) *p) {
                case '0': state = S72; break;
                case '1': state = S185; break;
                default:  return -1; /* leaf */
            }
            break;

            case S185: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000111" */
            switch ((unsigned char) *p) {
                case '0': state = S73; break;
                case '1': state = S186; break;
                default:  return -1; /* leaf */
            }
            break;

            case S186: /* e.g. "10000000000000000000000000000000000000000000000000000000000000001111" */
            switch ((unsigned char) *p) {
                case '0': state = S74; break;
                case '1': state = S187; break;
                default:  return -1; /* leaf */
            }
            break;

            case S187: /* e.g. "100000000000000000000000000000000000000000000000000000000000000011111" */
            switch ((unsigned char) *p) {
                case '0': state = S75; break;
                case '1': state = S188; break;
                default:  return -1; /* leaf */
            }
            break;

            case S188: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000111111" */
            switch ((unsigned char) *p) {
                case '0': state = S76; break;
                case '1': state = S189; break;
                default:  return -1; /* leaf */
            }
            break;

            case S189: /* e.g. "10000000000000000000000000000000000000000000000000000000000000001111111" */
            switch ((unsigned char) *p) {
                case '0': state = S77; break;
                case '1': state = S190; break;
                default:  return -1; /* leaf */
            }
            break;

            case S190: /* e.g. "100000000000000000000000000000000000000000000000000000000000000011111111" */
            switch ((unsigned char) *p) {
                case '0': state = S78; break;
                case '1': state = S191; break;
                default:  return -1; /* leaf */
            }
            break;

            case S191: /* e.g. "1000000000000000000000000000000000000000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S192; break;
                default:  return -1; /* leaf */
            }
            break;

            case S192: /* e.g. "10000000000000000000000000000000000000000000000000000000000000001111111111" */
            return -1; /* leaf */

            case S193: /* e.g. "100000000000000000000000000000000000000000000000000000011" */
            switch ((unsigned char) *p) {
                case '0': state = S62; break;
                case '1': state = S194; break;
                default:  return -1; /* leaf */
            }
            break;

            case S194: /* e.g. "1000000000000000000000000000000000000000000000000000000111" */
            switch ((unsigned char) *p) {
                case '0': state = S63; break;
                case '1': state = S195; break;
                default:  return -1; /* leaf */
            }
            break;

            case S195: /* e.g. "10000000000000000000000000000000000000000000000000000001111" */
            switch ((unsigned char) *p) {
                case '0': state = S64; break;
                case '1': state = S196; break;
                default:  return -1; /* leaf */
            }
            break;

            case S196: /* e.g. "100000000000000000000000000000000000000000000000000000011111" */
            switch ((unsigned char) *p) {
                case '0': state = S65; break;
                case '1': state = S197; break;
                default:  return -1; /* leaf */
            }
            break;

            case S197: /* e.g. "1000000000000000000000000000000000000000000000000000000111111" */
            switch ((unsigned char) *p) {
                case '0': state = S66; break;
                case '1': state = S198; break;
                default:  return -1; /* leaf */
            }
            break;

            case S198: /* e.g. "10000000000000000000000000000000000000000000000000000001111111" */
            switch ((unsigned char) *p) {
                case '0': state = S67; break;
                case '1': state = S199; break;
                default:  return -1; /* leaf */
            }
            break;

            case S199: /* e.g. "100000000000000000000000000000000000000000000000000000011111111" */
            switch ((unsigned char) *p) {
                case '0': state = S68; break;
                case '1': state = S200; break;
                default:  return -1; /* leaf */
            }
            break;

            case S200: /* e.g. "1000000000000000000000000000000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S201; break;
                default:  return -1; /* leaf */
            }
            break;

            case S201: /* e.g. "10000000000000000000000000000000000000000000000000000001111111111" */
            return -1; /* leaf */

            case S202: /* e.g. "100000000000000000000000000000000000000000000011" */
            switch ((unsigned char) *p) {
                case '0': state = S52; break;
                case '1': state = S203; break;
                default:  return -1; /* leaf */
            }
            break;

            case S203: /* e.g. "1000000000000000000000000000000000000000000000111" */
            switch ((unsigned char) *p) {
                case '0': state = S53; break;
                case '1': state = S204; break;
                default:  return -1; /* leaf */
            }
            break;

            case S204: /* e.g. "10000000000000000000000000000000000000000000001111" */
            switch ((unsigned char) *p) {
                case '0': state = S54; break;
                case '1': state = S205; break;
                default:  return -1; /* leaf */
            }
            break;

            case S205: /* e.g. "100000000000000000000000000000000000000000000011111" */
            switch ((unsigned char) *p) {
                case '0': state = S55; break;
                case '1': state = S206; break;
                default:  return -1; /* leaf */
            }
            break;

            case S206: /* e.g. "1000000000000000000000000000000000000000000000111111" */
            switch ((unsigned char) *p) {
                case '0': state = S56; break;
                case '1': state = S207; break;
                default:  return -1; /* leaf */
            }
            break;

            case S207: /* e.g. "10000000000000000000000000000000000000000000001111111" */
            switch ((unsigned char) *p) {
                case '0': state = S57; break;
                case '1': state = S208; break;
                default:  return -1; /* leaf */
            }
            break;

            case S208: /* e.g. "100000000000000000000000000000000000000000000011111111" */
            switch ((unsigned char) *p) {
                case '0': state = S58; break;
                case '1': state = S209; break;
                default:  return -1; /* leaf */
            }
            break;

            case S209: /* e.g. "1000000000000000000000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S210; break;
                default:  return -1; /* leaf */
            }
            break;

            case S210: /* e.g. "10000000000000000000000000000000000000000000001111111111" */
            return -1; /* leaf */

            case S211: /* e.g. "100000000000000000000000000000000000011" */
            switch ((unsigned char) *p) {
                case '0': state = S42; break;
                case '1': state = S212; break;
                default:  return -1; /* leaf */
            }
            break;

            case S212: /* e.g. "1000000000000000000000000000000000000111" */
            switch ((unsigned char) *p) {
                case '0': state = S43; break;
                case '1': state = S213; break;
                default:  return -1; /* leaf */
            }
            break;

            case S213: /* e.g. "10000000000000000000000000000000000001111" */
            switch ((unsigned char) *p) {
                case '0': state = S44; break;
                case '1': state = S214; break;
                default:  return -1; /* leaf */
            }
            break;

            case S214: /* e.g. "100000000000000000000000000000000000011111" */
            switch ((unsigned char) *p) {
                case '0': state = S45; break;
                case '1': state = S215; break;
                default:  return -1; /* leaf */
            }
            break;

            case S215: /* e.g. "1000000000000000000000000000000000000111111" */
            switch ((unsigned char) *p) {
                case '0': state = S46; break;
                case '1': state = S216; break;
                default:  return -1; /* leaf */
            }
            break;

            case S216: /* e.g. "10000000000000000000000000000000000001111111" */
            switch ((unsigned char) *p) {
                case '0': state = S47; break;
                case '1': state = S217; break;
                default:  return -1; /* leaf */
            }
            break;

            case S217: /* e.g. "100000000000000000000000000000000000011111111" */
            switch ((unsigned char) *p) {
                case '0': state = S48; break;
                case '1': state = S218; break;
                default:  return -1; /* leaf */
            }
            break;

            case S218: /* e.g. "1000000000000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S219; break;
                default:  return -1; /* leaf */
            }
            break;

            case S219: /* e.g. "10000000000000000000000000000000000001111111111" */
            return -1; /* leaf */

            case S220: /* e.g. "100000000000000000000000000011" */
            switch ((unsigned char) *p) {
                case '0': state = S32; break;
                case '1': state = S221; break;
                default:  return -1; /* leaf */
            }
            break;

            case S221: /* e.g. "1000000000000000000000000000111" */
            switch ((unsigned char) *p) {
                case '0': state = S33; break;
                case '1': state = S222; break;
                default:  return -1; /* leaf */
            }
            break;

            case S222: /* e.g. "10000000000000000000000000001111" */
            switch ((unsigned char) *p) {
                case '0': state = S34; break;
                case '1': state = S223; break;
                default:  return -1; /* leaf */
            }
            break;

            case S223: /* e.g. "100000000000000000000000000011111" */
            switch ((unsigned char) *p) {
                case '0': state = S35; break;
                case '1': state = S224; break;
                default:  return -1; /* leaf */
            }
            break;

            case S224: /* e.g. "1000000000000000000000000000111111" */
            switch ((unsigned char) *p) {
                case '0': state = S36; break;
                case '1': state = S225; break;
                default:  return -1; /* leaf */
            }
            break;

            case S225: /* e.g. "10000000000000000000000000001111111" */
            switch ((unsigned char) *p) {
                case '0': state = S37; break;
                case '1': state = S226; break;
                default:  return -1; /* leaf */
            }
            break;

            case S226: /* e.g. "100000000000000000000000000011111111" */
            switch ((unsigned char) *p) {
                case '0': state = S38; break;
                case '1': state = S227; break;
                default:  return -1; /* leaf */
            }
            break;

            case S227: /* e.g. "1000000000000000000000000000111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S228; break;
                default:  return -1; /* leaf */
            }
            break;

            case S228: /* e.g. "10000000000000000000000000001111111111" */
            return -1; /* leaf */

            default:
                ; /* unreached */
        }
    }

    /* end states */
    switch (state) {
        case S138: return 0x400; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S147: return 0x200; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S156: return 0x100; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S165: return 0x80; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S174: return 0x40; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S183: return 0x20; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S192: return 0x10; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S201: return 0x8; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S210: return 0x4; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S219: return 0x2; /* "1[01]{8}0[01]{8}0[01]{8}0[01]{8}01{10}" */
        case S228: return 0x1; /* "1[01]{8}0[01]{8}0[01]{8}01{10}" */
        default: return -1; /* unexpected EOT */
    }
}

