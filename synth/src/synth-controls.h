typedef enum SynthCtrlId_ {
	SCamp = 0,	// 00

	SCenv1amp,	// 01
	SCenv1dc,
	SCenv1atk,
	SCenv1dec,
	SCenv1sus,
	SCenv1rel,

	SCenv2amp,	// 07
	SCenv2dc,
	SCenv2atk,
	SCenv2dec,
	SCenv2sus,
	SCenv2rel,

	SCenv3amp,	// 13
	SCenv3dc,
	SCenv3atk,
	SCenv3dec,
	SCenv3sus,
	SCenv3rel,

	SCenv4amp,	// 19
	SCenv4dc,
	SCenv4atk,
	SCenv4dec,
	SCenv4sus,
	SCenv4rel,

	SClfo1amp,	// 25
	SClfo1fre,

	SClfo2amp,	// 27
	SClfo2fre,

	SCosc1amp,	// 29
	SCosc1fre,
	SCosc1note,
	SCosc1tune,
	SCosc1psw,
	SCosc1wave,

	SCosc2amp,	// 35
	SCosc2fre,
	SCosc2note,
	SCosc2tune,
	SCosc2psw,
	SCosc2wave,

	SCflt1amp,	// 41
	SCflt1cut,
	SCflt1res,
	SCflt1mod,
	SCflt1mode
} SynthCtrlId;