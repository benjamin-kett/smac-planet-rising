const result = {

	define: (game) => {
		const animations = [
			{
				id: 'ATTACK_PSI',
				data: {
					type: 'frames_row',
					file: 'xi.pcx',
					row_x: 2, row_y: 394,
					frame_width: 43, frame_height: 47,
					frame_padding: 2,
					frames_count: 16,
					scale_x: 0.7,
					scale_y: 0.7,
					duration_ms: 250,
					sound: 'psi attack.wav',
				}
			},
			{
				id: 'DEATH_PSI',
				data: {
					type: 'frames_row',
					file: 'xf.pcx',
					row_x: 2, row_y: 914,
					frame_width: 100, frame_height: 150,
					frame_center_x: 50, frame_center_y: 125,
					frame_padding: 2,
					frames_count: 16,
					frames_per_row: 8,
					scale_x: 1.0,
					scale_y: 1.0,
					duration_ms: 750,
					sound: 'psi blow.wav',
				}
			},
		];

		for (animation of animations) {
			game.event('define_animation', animation);
		}
		for (landing of [
			['PLANETFALL_HUMAN', 'drop_pod.png', 2201, ''],
			['PLANETFALL_ALIEN', 'alien_pod.png', 2201, ''],
			['PLANETFALL_SMOKE', 'drop_smoke.png', 620, 'crash land.wav'],
		]) {
			game.event('define_animation', {
				id: landing[0],
				data: {
					type: 'frames_row', file: landing[1],
					row_x: 0, row_y: 0,
					frame_width: 351, frame_height: 407,
					frame_center_x: 175, frame_center_y: 203,
					frame_padding: 0, frames_count: 31, frames_per_row: 4,
					scale_x: 3.51, scale_y: 4.07,
					duration_ms: landing[2], sound: landing[3],
				},
			});
		}

	},

};

return result;
