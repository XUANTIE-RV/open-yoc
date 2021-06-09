If you do not have a pair of signing/verifying keys, start from Step 1 to derive keys. Copy the verifying key to your firmware project. Keep your signing key properly.

If you have already generated a pair of keys, jump to Step 2 for signature.
    
Step 1:

    python3 key_gen.py

Step 2:

    python3 signing.py [ota_firmware].bin signing_key.pem

