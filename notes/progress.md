# 2022-09-22
- Checking if process has already been calculated by checking if the file exists.
- Problem: Does still have to test processes that don't work. This takes a long time in MARTY.
The fix would be to create an empty file every time a process didn't work.
This would however give quite a lot of files.
Writing into a log file might run into race conditions with the parallelization.
