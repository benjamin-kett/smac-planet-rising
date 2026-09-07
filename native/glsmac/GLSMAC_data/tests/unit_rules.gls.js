const definitions = #include('../default/units/defs');
let rules = {units: [], native_morale: [], conventional_morale: []};
let i = 0;
while (i < 23) {
    rules.units :+ {name: 'Test ' + #to_string(i), offense: -1, defense: -1,
        reactor: 1, cost: i, role: 2, abilities: 0};
    i++;
}
i = 0;
while (i < 7) {
    rules.native_morale :+ 'N' + #to_string(i);
    rules.conventional_morale :+ 'C' + #to_string(i);
    i++;
}
let events = [];
let saved = null;
definitions.define({
    get_um: () => { return {get_rules: () => { return rules; }}; },
    set: (key, value) => { test.assert(key == 'unit_catalog'); saved = value; },
    event: (name, data) => { events :+ {name: name, data: data}; },
});
test.assert(saved == rules);
test.assert(#sizeof(events) == 6);
test.assert(events[0].data.id == 'NATIVE');
test.assert(events[1].data.id == 'CONVENTIONAL');
test.assert(#sizeof(events[1].data.data) == 7);
test.assert(events[2].data.id == 'FungalTower' && events[2].data.data.cost == 19);
test.assert(events[3].data.id == 'MindWorms' && events[3].data.data.cost == 8);
test.assert(events[4].data.id == 'SeaLurk' && events[4].data.data.cost == 14);
test.assert(events[5].data.id == 'SporeLauncher' && events[5].data.data.cost == 15);
