# Testes do comparador do gate de cutover.
import subprocess, sys, textwrap, pathlib, tempfile

HERE = pathlib.Path(__file__).parent
HDR = "task\tprp\tgen\trep\tverdict\ttag\tline\tfunction\twitness\twall_s\tmaxrss_kb\n"

def run(c_rows, r_rows, accepted=""):
    d = pathlib.Path(tempfile.mkdtemp())
    (d / "c.tsv").write_text(HDR + "".join(c_rows))
    (d / "r.tsv").write_text(HDR + "".join(r_rows))
    (d / "a.tsv").write_text("task\tprp\tgen\tfield\tc_value\trust_value\tsd_ref\n" + accepted)
    return subprocess.run([sys.executable, HERE / "compare.py", d / "c.tsv", d / "r.tsv",
                           "--accepted", d / "a.tsv"], capture_output=True, text=True)

ROW = "t.yml\tvalid-memsafety\tafl\t{rep}\t{v}\t{tag}\t7\tmain\t0\t{wall}\t1000\n"

def rows(v="FAILED", tag="FALSE-DEREF", wall=1.0):
    return [ROW.format(rep=i, v=v, tag=tag, wall=wall) for i in range(1, 6)]

def test_identical_passes():
    assert run(rows(), rows()).returncode == 0

def test_verdict_change_blocks():
    p = run(rows(), rows(v="UNKNOWN", tag="-"))
    assert p.returncode == 1 and "verdict" in p.stdout

def test_accepted_delta_passes():
    acc = "t.yml\tvalid-memsafety\tafl\ttag\tFALSE-DEREF\tFALSE-FREE\tSD-06\n"
    assert run(rows(), rows(tag="FALSE-FREE"), acc).returncode == 0

def test_slowdown_over_10_percent_blocks():
    p = run(rows(wall=1.0), rows(wall=1.2))
    assert p.returncode == 1 and "wall" in p.stdout

def test_new_timeout_blocks():
    p = run(rows(), rows(v="TIMEOUT", tag="-"))
    assert p.returncode == 1 and "TIMEOUT" in p.stdout

def test_missing_task_blocks():
    p = run(rows(), [])
    assert p.returncode == 1 and "missing" in p.stdout
