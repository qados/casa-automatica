with open(r"C:\Users\Lucas\Projetos\servidor\index.html", "r", encoding="utf-8") as f:
    g = f.read()

cLines = g.replace("\"", "\'")
cLines = cLines.replace("\n", " \\\n")

with open("fixed.txt", "w", encoding="utf-8") as j:
    j.write(cLines)

j.close()
f.close()