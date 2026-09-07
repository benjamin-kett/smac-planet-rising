return (i) => {
	const show_factions = (title, factions) => {
		let entries = [];
		for (faction of factions) {
			const id = faction[0];
			entries :+[faction[1], () => {
				i.glsmac.start_game(id);
			}];
		}
		i.sliding.show({title: title, entries: entries});
	};
	i.sliding.show({
		title: 'Choose your faction',
		entries: [
			['Alien Crossfire factions', () => {
				show_factions('Alien Crossfire factions', [
					['CONSCIOUSNESS', 'Cybernetic Consciousness'],
					['PIRATES', 'Nautilus Pirates'],
					['DRONES', 'Free Drones'],
					['ANGELS', 'Data Angels'],
					['PLANETCULT', 'Cult of Planet'],
					['CARETAKERS', 'Manifold Caretakers'],
					['USURPERS', 'Manifold Usurpers'],
				]);
			}],
			['Original factions', () => {
				show_factions('Original factions', [
					['GAIANS', 'Gaia\'s Stepdaughters'],
					['HIVE', 'Human Hive'],
					['UNIVERSITY', 'University of Planet'],
					['MORGANITES', 'Morgan Industries'],
					['SPARTANS', 'Spartan Federation'],
					['BELIEVERS', 'Lord\'s Believers'],
					['PEACEKEEPERS', 'Peacekeeping Forces'],
				]);
			}],
			['Random faction', () => { i.glsmac.start_game(); }],
		],
	});
};
