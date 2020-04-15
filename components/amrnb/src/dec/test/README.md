## Media Testing ##
---
#### AMR-NB Decoder :
The Amr-Nb Decoder Test Suite validates the amrnb decoder available in libstagefright.

Run the following steps to build the test suite:
```
m AmrnbDecoderTest
```

The 32-bit binaries will be created in the following path : ${OUT}/data/nativetest/

The 64-bit binaries will be created in the following path : ${OUT}/data/nativetest64/

To test 64-bit binary push binaries from nativetest64.
```
adb push ${OUT}/data/nativetest64/AmrnbDecoderTest/AmrnbDecoderTest /data/local/tmp/
```

To test 32-bit binary push binaries from nativetest.
```
adb push ${OUT}/data/nativetest/AmrnbDecoderTest/AmrnbDecoderTest /data/local/tmp/
```

The resource file for the tests is taken from [here](https://drive.google.com/drive/folders/13cM4tAaVFrmr-zGFqaAzFBbKs75pnm9b). Push these files into device for testing.
Download amr-nb folder and push all the files in this folder to /data/local/tmp/ on the device.
```
adb push amr-nb/. /data/local/tmp/
```

usage: AmrnbDecoderTest -P \<path_to_folder\>
```
adb shell /data/local/tmp/AmrnbDecoderTest -P /data/local/tmp/
```
