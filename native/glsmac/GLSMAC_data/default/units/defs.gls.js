const native_lifeform = (id, name, movement_type, movement_per_turn, base_y, rules_index) => {
	return {
		id: id,
		rules_index: rules_index,
		data: {
			name: name,
			morale: 'NATIVE',
			type: 'static',
			movement_type: movement_type,
			movement_per_turn: movement_per_turn,
			render: {
				type: 'sprite',
				file: 'units.pcx',
				x: 2, y: base_y,
				w: 100, h: 75,
				cx: 53, cy: base_y + 51,
				morale_based_xshift: 102
			},
		},
	};
};

const units = [
	native_lifeform('FungalTower', 'Fungal Tower', 'immovable', 0, 79, 19),
	native_lifeform('MindWorms', 'Mind Worms', 'land', 1, 233, 8),
	native_lifeform('SeaLurk', 'Sea Lurk', 'water', 4, 310, 14),
	native_lifeform('SporeLauncher', 'Spore Launcher', 'land', 1, 387, 15),
];

const result = {
	define: (game) => {

		const rules = game.get_um().get_rules();
		game.set('unit_catalog', rules);
		let native_morale = [];
		let conventional_morale = [];
		for (name of rules.native_morale) { native_morale :+ {name: name}; }
		for (name of rules.conventional_morale) { conventional_morale :+ {name: name}; }
		game.event('define_moraleset', {id: 'NATIVE', data: native_morale});
		game.event('define_moraleset', {id: 'CONVENTIONAL', data: conventional_morale});

		for (unit of units) {
			const rule = rules.units[unit.rules_index];
			const data = unit.data;
			data.name = rule.name;
			data.offense = rule.offense;
			data.defense = rule.defense;
			data.reactor = rule.reactor;
			data.cost = rule.cost;
			data.role = rule.role;
			data.abilities = rule.abilities;
			data.is_native = true;
			game.event('define_unit', {id: unit.id, data: data});
		}

	},
};

return result;
