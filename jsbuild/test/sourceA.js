include('sourceB.js');
include('sourceC.js');

//This is source A
(function() {
	function SourceA() {
	}
	
	publish(SourceA, 'SourceA');
})();