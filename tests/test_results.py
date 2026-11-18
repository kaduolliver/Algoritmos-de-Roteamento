"""Verifica automaticamente os dois executaveis nos cenarios inicial e final."""
import os
import re
import subprocess
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXPECTED_INITIAL = [
    [0, 3, 2, 8, 10],
    [3, 0, 1, 5, 7],
    [2, 1, 0, 6, 8],
    [8, 5, 6, 0, 2],
    [10, 7, 8, 2, 0],
]
EXPECTED_FINAL = [
    [0, 3, 2, 5, 7],
    [3, 0, 1, 2, 4],
    [2, 1, 0, 3, 5],
    [5, 2, 3, 0, 2],
    [7, 4, 5, 2, 0],
]


def run(executable):
    binary = ROOT / (executable + (".exe" if os.name == "nt" else ""))
    return subprocess.check_output([str(binary)], text=True)


def parse_tables(output, name, algorithm):
    tag = f"=== CENARIO {name}: {algorithm} ==="
    section = output.split(tag, 1)[1].split("ALTERACAO:", 1)[0]
    table_rows = re.findall(r"([A-E]) -> ([A-E])\s*\|\s*(\d+)\s*\|\s*(local|[A-E])", section)
    result = {}
    for source, dest, distance, hop in table_rows:
        result[(source, dest)] = (int(distance), hop)
    return result


class RoutingTests(unittest.TestCase):
    def test_expected_metrics_and_next_hops(self):
        for executable, alg in [("bellman_ford", "BELLMAN-FORD"), ("dijkstra", "DIJKSTRA")]:
            output = run(executable)
            for scenario, expected in [("INICIAL", EXPECTED_INITIAL), ("FINAL", EXPECTED_FINAL)]:
                rows = parse_tables(output, scenario, alg)
                self.assertEqual(len(rows), 25, (executable, scenario))
                for i, a in enumerate("ABCDE"):
                    for j, b in enumerate("ABCDE"):
                        distance, hop = rows[(a, b)]
                        self.assertEqual(distance, expected[i][j], (executable, scenario, a, b))
                        self.assertEqual(distance, rows[(b, a)][0], "Topologia bidirecional")
                        if a == b:
                            self.assertEqual(hop, "local")
                        else:
                            self.assertIn(hop, "ABCDE")
                self.assertEqual(rows[("A", "E")][1], "C")
                self.assertEqual(rows[("B", "E")][1], "D")
                self.assertEqual(rows[("C", "D")][1], "B")

    def test_algorithms_agree(self):
        bf = run("bellman_ford")
        dj = run("dijkstra")
        for scenario in ("INICIAL", "FINAL"):
            self.assertEqual(
                parse_tables(bf, scenario, "BELLMAN-FORD"),
                parse_tables(dj, scenario, "DIJKSTRA"),
            )

    def test_requirements_and_traces(self):
        bf = run("bellman_ford")
        dj = run("dijkstra")
        for app_output in (bf, dj):
            for router, degree in zip("ABCDE", (2, 3, 4, 3, 2)):
                self.assertIn(f"{router}: ", app_output)
                self.assertIn(f"({degree} interfaces/enlaces)", app_output)
        self.assertIn("Rodada 0", bf)
        self.assertIn("Rodada 3", bf)
        self.assertEqual(dj.count("Passo 5: fixa E"), 2)


if __name__ == "__main__":
    unittest.main(verbosity=2)
