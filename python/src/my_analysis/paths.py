"""Path naming shared by Python-side analysis utilities."""


def run_tag(run_number: int) -> str:
    if run_number < 0:
        raise ValueError("run number must be non-negative")
    return f"run{run_number:05d}"

