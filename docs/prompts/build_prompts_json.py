#!/usr/bin/env python3
"""Extrai os prompts de deepseek-property-prompts.md para JSON.

O markdown e a unica fonte de verdade; este script so o transcreve para o
formato que o harness consome, para que os dois nunca divirjam.
"""
import json
import pathlib
import re

HERE = pathlib.Path(__file__).parent
MD = HERE / "deepseek-property-prompts.md"
OUT = HERE / "prompts.json"

text = MD.read_text(encoding="utf-8")

# Blocos de prompt sao fences de 4 crases marcadas como `text`.
blocks = re.findall(r"^````text\n(.*?)^````$", text, re.M | re.S)
if len(blocks) != 9:
    raise SystemExit(f"esperava 9 blocos (1 system + 8 prompts), achei {len(blocks)}")

system, *user_blocks = blocks

# Cabecalhos "## Pn — VERDICT (descricao)" e seus metadados.
sections = re.findall(
    r"^## (P\d) — `([A-Z-]+)`.*?\n"
    r".*?\*\*Map2Check:\*\* `([^`]+)`.*?\n"
    r"- \*\*CWEs no benchmark:\*\* ([^\n]*)\n"
    r".*?\*\*Tokens permitidos:\*\* ([^\n]+)\n",
    text, re.M | re.S)
if len(sections) != 8:
    raise SystemExit(f"esperava 8 secoes, achei {len(sections)}")

def cwes(raw):
    """CWEs rotulados nos runners do repositorio. Vazio e um resultado valido:
    significa que a trilha nao tem caso rotulado e precisa de corpus proprio."""
    if "*nenhum*" in raw:
        return []
    return [int(n) for n in re.findall(r"\d+", raw)]

def tokens(raw):
    return re.findall(r"`([A-Z-]+)`", raw)

prompts = []
for (pid, verdict, cmd, cwe_raw, tok_raw), body in zip(sections, user_blocks):
    prompts.append({
        "id": pid,
        "verdict": verdict,
        "map2check_command": cmd,
        "benchmark_cwes": cwes(cwe_raw),
        "allowed_tokens": tokens(tok_raw),
        "placeholders": sorted(set(re.findall(r"\{\{(\w+)\}\}", body))),
        "user_prompt": body.rstrip("\n"),
    })

OUT.write_text(json.dumps(
    {"system_prompt": system.rstrip("\n"), "prompts": prompts},
    ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
print(f"{OUT.name}: {len(prompts)} prompts")
