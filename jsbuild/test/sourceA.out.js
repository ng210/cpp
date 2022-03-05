/* d:\code\git\cpp\jsbuild\test\sourceD.js ***********************************/
// include('sourceC.js');
// include('sourceF.js');

//This is source D
(function() {
	function SourceD() {
	}
	function SourceD1() {
	}
	
// 	publish(SourceD, 'SourceD');
	self['SourceD'] = SourceD;
// 	publish(SourceD1, 'SourceD1');
	self['SourceD1'] = SourceD1;
})();
/* d:\code\git\cpp\jsbuild\test\sourceB.js ***********************************/
// include('sourceD.js');

//This is source B
(function() {
	function SourceB() {
	}
	
// 	publish(SourceB, 'SourceB');
	self['SourceB'] = SourceB;
})();
/* d:\code\git\cpp\jsbuild\test\sourceC.js ***********************************/
// include('sourceD.js');
// include('sourceE.js');
// include('sourceF.js');
// include('sourceH.js');

//This is source C
(function() {
	function SourceC() {
	}
	
// 	publish(SourceC, 'SourceC');
	self['SourceC'] = SourceC;
})();
/* d:/code/git/cpp/jsbuild/test/sourceA.js ***********************************/
// include('sourceB.js');
// include('sourceC.js');

//This is source A
(function() {
	function SourceA() {
	}
	
// 	publish(SourceA, 'SourceA');
	self['SourceA'] = SourceA;
})();