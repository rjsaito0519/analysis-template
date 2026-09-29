import pytest

from my_analysis.paths import run_tag


def test_run_tag_is_zero_padded() -> None:
    assert run_tag(42) == "run00042"


def test_run_tag_rejects_negative_values() -> None:
    with pytest.raises(ValueError):
        run_tag(-1)

