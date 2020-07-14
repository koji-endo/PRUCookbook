for i in range(22, -1, -1):
    stri = f"""  AND r7,r6,1
  OR r30, r8, r7
  delay
  LSR r6, r14, {i}
  OR r30, r9, r7
  delay
"""
    print(stri)
