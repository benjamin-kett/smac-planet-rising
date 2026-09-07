const select_faction = #include('../default/ui/parts/mainmenu/steps/select_faction');
let menu = null;
let selected = '';
const input = {
	sliding: {show: (data) => { menu = data; }},
	glsmac: {start_game: (id) => { selected = id; }},
};
select_faction(input);
const groups = menu.entries;
const expected = [
	['CONSCIOUSNESS', 'PIRATES', 'DRONES', 'ANGELS', 'PLANETCULT', 'CARETAKERS', 'USURPERS'],
	['GAIANS', 'HIVE', 'UNIVERSITY', 'MORGANITES', 'SPARTANS', 'BELIEVERS', 'PEACEKEEPERS'],
];
let group = 0;
while (group < 2) {
	groups[group][1]();
	const entries = menu.entries;
	test.assert(#sizeof(entries) == 7);
	let index = 0;
	while (index < 7) {
		entries[index][1]();
		test.assert(selected == expected[group][index]);
		index++;
	}
	group++;
}

// Verify ownership, startup-only validation, faction-specific sequence and tile binding.
const planetfall = #include('../default/game/event/planetfall');
for (alien of [false, true]) {
    let sequence = [];
    let turn = 1;
    const tile = {is_locked: () => { return false; }};
    const event = {
        caller: 0,
        data: {unit: {owner: 0, get_tile: () => { return tile; }}},
        game: {
            get_turn: () => { return turn; },
            get_player: (id) => { return {get_faction: () => { return {is_progenitor: alien}; }}; },
            am: {show_animations: (animations) => { sequence = animations; return 7; }},
        },
    };
    test.assert(!#is_defined(planetfall.validate(event)));
    test.assert(planetfall.apply(event).animations_id == 7);
    test.assert(#sizeof(sequence) == 2);
    test.assert(sequence[0].id == (alien ? 'PLANETFALL_ALIEN' : 'PLANETFALL_HUMAN'));
    test.assert(sequence[0].tile == tile);
    test.assert(sequence[1].id == 'PLANETFALL_SMOKE');
    turn = 2;
    test.assert(#is_defined(planetfall.validate(event)));
    turn = 1;
    event.data.unit.owner = 1;
    test.assert(#is_defined(planetfall.validate(event)));
}
