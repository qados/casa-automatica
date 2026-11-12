import minify_html

with open(r"C:\Users\Lucas\Projetos\main\index.html", "r", encoding="utf-8") as f:
    g = f.read()

minified = minify_html.minify(g, minify_js=True, minify_css=True, remove_processing_instructions=True)
cLines = minified.replace("\"", "\'")
cLines = cLines.replace("\n", " \\\n")

with open("fixed.txt", "w", encoding="utf-8") as j:
    j.write(cLines)

j.close()
f.close()