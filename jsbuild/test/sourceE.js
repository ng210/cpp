include('sourceD.js');
//This is source E
(function() {
	function SourceE() {
	}
	
	publish(SourceE, 'SourceE', SourceD);
})();