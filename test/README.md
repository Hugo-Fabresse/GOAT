# GOAT - Test Suite

This folder contains the unit tests for the GOAT project, based on Criterion.

## Running Unit Tests

From the root of the project, run:

```bash
make tests
./build/init_tests

```

This will display the test results in the terminal.

## Generating and Viewing Code Coverage

To generate a coverage report:

```bash
make coverage
```

The HTML report will be created in the `coverage/` folder.

To view it:

```bash
xdg-open coverage/index.html
```

## Dependencies

- [Criterion](https://criterion.readthedocs.io/en/master/)
- [lcov](http://ltp.sourceforge.net/coverage/lcov.php)
- `genhtml` (included with lcov)

Install them if necessary:

```bash
sudo apt update
sudo apt install libcriterion-dev lcov
```

## Cleaning Up

To remove all build and coverage files:

```bash
make distclean
```

---

**Note:** The tests delete and recreate the .goat folder each time they run to ensure the independence of the test cases.
