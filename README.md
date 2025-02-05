PHISON Lab
======

Compile
------

To complie this project, you need to run the shell command down below.

`sh make_ssd.sh`

Run
------

If you want to run the project, please make sure you have the ssd folder in the `/tmp`.

If not, you can make the folder by this command.

`mkdir /tmp/ssd/`

After the folder was created, you can use the command down below to run the project.

`./ssd_fuse -d /tmp/ssd/`

Test Cases
------

If you want to test the project, you can test by youself with the `ssd_fuse_dut`.

To see the usage, run `./ssd_fuse_dut`.

And we prepare our testcases in the bash file, if you want to try it out, do

`bash testcase.sh`

The bash will echo ten thousands time to the SSD, and verify the   correctness by finding with differences from txts.