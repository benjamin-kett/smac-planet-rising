// Export candidate routines for local native-port research. No game bytes in this script.
// Addresses originate from the pinned Thinker symbol map; correspondence must be reviewed.
// @category SMAC.NativePort
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.charset.StandardCharsets;

public class ExportPortSlice extends GhidraScript {
    @Override public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) throw new IllegalArgumentException("output directory required");
        Path output = Path.of(args[0]);
        Files.createDirectories(output);
        StringBuilder index = new StringBuilder("address\tname\tbytes\n");
        for (Function function : currentProgram.getFunctionManager().getFunctions(true)) {
            index.append(function.getEntryPoint()).append('\t').append(function.getName())
                .append('\t').append(function.getBody().getNumAddresses()).append('\n');
        }
        Files.writeString(output.resolve("functions.tsv"), index, StandardCharsets.UTF_8);
        DecompInterface decompiler = new DecompInterface();
        if (!decompiler.openProgram(currentProgram)) throw new IllegalStateException("cannot open decompiler");
        try {
            String[][] targets = {
                {"004c1d20", "candidate_vehdraw_construct_vehicle"},
                {"004c2030", "candidate_vehdraw_construct_abil"},
                {"004c2360", "candidate_vehdraw_construct_vehicle_2"},
                {"004c2470", "candidate_vehdraw_construct_chassis"},
                {"004c2cf0", "candidate_vehdraw_construct_weapon"},
                {"004c3090", "candidate_vehdraw_construct_armor"},
                {"004c3800", "candidate_vehdraw_construct_reactor"},
            };
            if (args.length > 1) {
                targets = new String[args.length - 1][];
                for (int i = 1; i < args.length; ++i) {
                    String[] target = args[i].split(":", 2);
                    if (target.length != 2 || !target[1].matches("[a-zA-Z0-9_]+")) throw new IllegalArgumentException("address:label expected");
                    targets[i - 1] = target;
                }
            }
            for (String[] target : targets) {
                monitor.checkCancelled();
                Address address = toAddr(target[0]);
                Function function = currentProgram.getFunctionManager().getFunctionAt(address);
                if (function == null) { println("No function at candidate " + target[0]); continue; }
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                if (!result.decompileCompleted()) { println("Decompile failed: " + result.getErrorMessage()); continue; }
                String code = "// CANDIDATE " + target[1] + " at " + address + "\n"
                    + "// Original executable correspondence and inferred types need review.\n"
                    + result.getDecompiledFunction().getC();
                Files.writeString(output.resolve(target[1] + ".c"), code, StandardCharsets.UTF_8);
                println("Exported " + target[1]);
            }
        } finally { decompiler.dispose(); }
    }
}
