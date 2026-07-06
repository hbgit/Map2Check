#!/usr/bin/env python3
"""
TestComp 2026 Checkpoint — Execution Script for Map2Check v8.

Runs Map2Check v8 inside Docker (map2check-dev) against sv-benchmarks.
Adapted from guilhermeToCheck/test-comp2023/simulation/script_execucao.py.

Usage:
    python3 script_execucao.py [--smoke-test]
"""

import os
import subprocess
import xml.etree.ElementTree as ET
import yaml
import time
import glob
import argparse
import csv
import sys

# --- Configuration ---
DOCKER_IMAGE = "map2check-dev"
WORKSPACE_MOUNT = "/workspace"
MAP2CHECK_BUILD = "/workspace/build/modules/frontend/map2check"
PASSES_DIR = "/workspace/build/modules/backend/pass"
TIMEOUT_SECONDS = 300
PROPERTY_FILE = "coverage-error-call.prp"

# Paths inside the Docker container
DOCKER_RELEASE_DIR = "/workspace/test-comp2026/simulation/release"
DOCKER_SV_BENCHMARKS = "/workspace/test-comp2026/simulation/sv-benchmarks"


def run_in_docker(command, cwd_inside=None, timeout=TIMEOUT_SECONDS + 30):
    """Execute a command inside the map2check-dev Docker container."""
    docker_cmd = [
        "docker", "run", "--rm",
        "-v", "{}:{}".format(os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")), WORKSPACE_MOUNT),
        "-e", "MAP2CHECK_PATH={}".format(DOCKER_RELEASE_DIR),
        DOCKER_IMAGE,
        "bash", "-c", command
    ]
    try:
        result = subprocess.run(
            docker_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout
        )
        return result.stdout.decode('utf-8', errors='replace'), result.stderr.decode('utf-8', errors='replace'), result.returncode
    except subprocess.TimeoutExpired:
        return "Timed out", "", 1
    except Exception as e:
        return "", str(e), 1


def setup_release_dir():
    """Copy map2check binary and passes into the release directory."""
    print("[INFO] Setting up release directory with map2check v8 binaries...")
    setup_cmd = (
        "cp {build} {release}/map2check && "
        "mkdir -p {release}/lib && "
        "cp {passes}/lib*.so {release}/lib/ && "
        "chmod +x {release}/map2check"
    ).format(
        build=MAP2CHECK_BUILD,
        release=DOCKER_RELEASE_DIR,
        passes=PASSES_DIR
    )
    stdout, stderr, rc = run_in_docker(setup_cmd, timeout=30)
    if rc != 0:
        print("[ERRO] Failed to setup release dir: {}".format(stderr))
        sys.exit(1)
    print("[INFO] Release directory ready.")


def ler_includesfile(includesfile_path):
    """Read .set file and return list of subcategory glob patterns."""
    subcategorias = []
    if not os.path.exists(includesfile_path):
        print("[ERRO] Includes file not found: {}".format(includesfile_path))
        return subcategorias
    with open(includesfile_path, "r") as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#"):
                subcategorias.append(line)
    return subcategorias


def extrair_input_files(yml_path):
    """Extract input_files from a .yml task definition."""
    try:
        with open(yml_path, "r") as f:
            data = yaml.safe_load(f)
            if data and "input_files" in data:
                raw_input = data["input_files"].strip("'")
                return raw_input
    except Exception as e:
        print("[ERRO] Error reading {}: {}".format(yml_path, e))
    return None


def executar_task(input_file_rel, property_name):
    """Run map2check on a single input file inside Docker."""
    # Build the command to run inside Docker
    wrapper_cmd = (
        "cd {release} && "
        "python3 map2check-wrapper.py -p {prop} {input_file}"
    ).format(
        release=DOCKER_RELEASE_DIR,
        prop="/workspace/test-comp2026/simulation/{}".format(property_name),
        input_file="{}/c/{}".format(DOCKER_SV_BENCHMARKS, input_file_rel)
    )

    inicio = time.time()
    stdout, stderr, rc = run_in_docker(wrapper_cmd, timeout=TIMEOUT_SECONDS + 30)
    fim = time.time()
    tempo = fim - inicio

    # Parse result from stdout
    resultado = "UNKNOWN"
    for line in stdout.strip().splitlines():
        line = line.strip()
        if line in ["FALSE", "FALSE_FREE", "FALSE_DEREF", "FALSE_MEMTRACK",
                     "FALSE_MEMCLEANUP", "FALSE_OVERFLOW", "TRUE", "UNKNOWN"]:
            resultado = line
        elif line == "Timed out":
            resultado = "TIMEOUT"

    return tempo, resultado


def processar_tarefas(xml_file, resultados_dir, smoke_test=False):
    """Process all tasks defined in the benchmark XML."""
    tree = ET.parse(xml_file)
    root = tree.getroot()

    # Determine property file from rundefinition
    property_name = PROPERTY_FILE

    all_results = []
    task_count = 0

    for tasks_elem in root.findall("tasks"):
        categoria = tasks_elem.get("name")
        includesfile_rel = tasks_elem.find("includesfile").text

        # Resolve includesfile path
        sim_dir = os.path.dirname(os.path.abspath(xml_file))
        includesfile_path = os.path.join(sim_dir, includesfile_rel)

        print("\n" + "=" * 60)
        print("[INFO] Categoria: {}".format(categoria))
        print("=" * 60)

        subcategorias = ler_includesfile(includesfile_path)
        if not subcategorias:
            print("[WARN] No subcategories found for {}".format(categoria))
            continue

        # Determine property for this category
        if "CoverBranches" in categoria:
            prop = "coverage-branches.prp"
        else:
            prop = "coverage-error-call.prp"

        cat_dir = os.path.join(resultados_dir, categoria)
        os.makedirs(cat_dir, exist_ok=True)

        tempos = []

        for subcat_pattern in subcategorias:
            # Find .yml files matching the pattern
            sv_bench_c = os.path.join(sim_dir, "sv-benchmarks", "c")
            yml_pattern = os.path.join(sv_bench_c, subcat_pattern)
            yml_files = sorted(glob.glob(yml_pattern))

            if not yml_files:
                continue

            for yml_file in yml_files:
                input_file_rel = extrair_input_files(yml_file)
                if not input_file_rel:
                    continue

                # Resolve relative path from yml location
                yml_dir = os.path.dirname(yml_file)
                yml_dir_rel = os.path.relpath(yml_dir, sv_bench_c)
                full_input_rel = os.path.join(yml_dir_rel, input_file_rel)

                task_name = os.path.basename(input_file_rel)
                print("[RUN] {}/{}: {}".format(categoria, yml_dir_rel, task_name), end=" ... ", flush=True)

                tempo, resultado = executar_task(full_input_rel, prop)
                print("{} ({:.1f}s)".format(resultado, tempo))

                tempos.append((task_name, tempo, resultado))
                all_results.append((categoria, task_name, tempo, resultado))

                task_count += 1
                if smoke_test and task_count >= 5:
                    print("\n[INFO] Smoke test limit reached (5 tasks). Stopping.")
                    return all_results

        # Write per-category results
        tempos_file = os.path.join(cat_dir, "tempos.txt")
        with open(tempos_file, "w") as f:
            for arquivo, tempo, resultado in tempos:
                f.write("{}: {:.3f}s, Result: {}\n".format(arquivo, tempo, resultado))
        print("[INFO] Results saved to {}".format(tempos_file))

    return all_results


def gerar_relatorio(all_results, output_file):
    """Generate CSV summary report."""
    with open(output_file, "w", newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["category", "file", "time_s", "result"])
        for cat, name, tempo, resultado in all_results:
            writer.writerow([cat, name, "{:.3f}".format(tempo), resultado])

    # Summary stats
    total = len(all_results)
    solved = sum(1 for _, _, _, r in all_results if r in ["FALSE", "TRUE"])
    unknown = sum(1 for _, _, _, r in all_results if r == "UNKNOWN")
    timeout = sum(1 for _, _, _, r in all_results if r == "TIMEOUT")
    errors = total - solved - unknown - timeout

    print("\n" + "=" * 60)
    print("SUMMARY REPORT")
    print("=" * 60)
    print("Total tasks:  {}".format(total))
    print("Solved:       {} ({:.1f}%)".format(solved, 100 * solved / max(total, 1)))
    print("Unknown:      {} ({:.1f}%)".format(unknown, 100 * unknown / max(total, 1)))
    print("Timeout:      {} ({:.1f}%)".format(timeout, 100 * timeout / max(total, 1)))
    print("Errors:       {} ({:.1f}%)".format(errors, 100 * errors / max(total, 1)))
    print("Report saved: {}".format(output_file))


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="TestComp 2026 Checkpoint Runner")
    ap.add_argument("--smoke-test", action="store_true",
                    help="Run only 5 tasks as smoke test")
    ap.add_argument("--xml", default="map2check-v8.xml",
                    help="Benchmark XML file (default: map2check-v8.xml)")
    cli_args = ap.parse_args()

    sim_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(sim_dir)

    xml_file = cli_args.xml
    resultados_dir = "resultados_de_testes"
    os.makedirs(resultados_dir, exist_ok=True)

    print("=" * 60)
    print("TestComp 2026 Checkpoint — Map2Check v8 (LLVM 16)")
    print("=" * 60)
    print("XML:         {}".format(xml_file))
    print("Results dir: {}".format(os.path.abspath(resultados_dir)))
    print("Smoke test:  {}".format(cli_args.smoke_test))
    print("Timeout:     {}s per task".format(TIMEOUT_SECONDS))
    print()

    # Setup release directory with binaries
    setup_release_dir()

    # Run tasks
    all_results = processar_tarefas(xml_file, resultados_dir,
                                    smoke_test=cli_args.smoke_test)

    # Generate report
    report_file = os.path.join(resultados_dir, "summary_report_v8.csv")
    gerar_relatorio(all_results, report_file)
