return (p) => {
	let pending = p.game.get_turn() == 1;
	p.root.listen(p.game, 'unit_select', (e) => {
		if (pending) {
			pending = false;
			const unit = e.unit;
			// Let the initial 100 ms camera pan finish before planetfall starts.
			p.root.timer(150, () => {
				p.game.event('planetfall', {unit: unit});
				return false;
			});
		}
	});
};
