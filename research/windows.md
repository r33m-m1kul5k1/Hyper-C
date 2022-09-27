# Windows

## Entering password flow

- credential provider
- rpc (password structure) -> lsa
- lsa gives the password to cred guard
- cred guard gives in return a blob
- SSP - authentication packages (NTLM, negotiate (chose between NTLM and Kerberos), Kerberos, msv1_0)

### Kerberos

1. AS-REQ (request the TGT)
   - Active Directory (information about privileges of each user) -> TGT (diffie-hellman between the KDC and the client for passing the TGT.)

2. TGS-REQ
   - AD
   - encrypted ticket
   - lsa decrypt the key passing the blob and the encrypted ticket to cred guard
   -> gets the PAC (user details (group, name, etc..)) from cred guard and generate a logon token
