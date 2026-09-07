// Restore presentation at the real starting tile. The original starting-unit
// roster and delayed Progenitor arrival are separate, unfinished rules work.
return {
	validate: (e) => {
		if (e.game.get_turn() != 1) { return 'Planetfall is only available at game start'; }
		if (e.data.unit.owner != e.caller) { return 'Cannot land another player\'s unit'; }
		if (e.data.unit.get_tile().is_locked()) { return 'Landing tile is busy'; }
	},
	apply: (e) => {
		const faction = e.game.get_player(e.caller).get_faction();
		const id = faction.is_progenitor ? 'PLANETFALL_ALIEN' : 'PLANETFALL_HUMAN';
		const tile = e.data.unit.get_tile();
		#print('Planetfall animation started: ' + id);
		return {animations_id: e.game.am.show_animations([
			{id: id, tile: tile},
			{id: 'PLANETFALL_SMOKE', tile: tile, oncomplete: () => {
				#print('Planetfall animation completed: ' + id);
			}},
		])};
	},
	rollback: (e) => { e.game.am.stop_animations(e.applied.animations_id); },
};
