from io import BytesIO
import os
from typing import Optional
from bottle import route, post, run, request, FileUpload, response, static_file

import tempfile
import re
import subprocess
import glob

import zipfile

CONFIG = r"""
/*
cmarshal:`{
	"cJSONInclude": "./cJSON.h",
	"unmarshalerHeaderFile": "./unmarshaler.h",
	"unmarshalerImplFile": "./unmarshaler.c",
	"marshalerHeaderFile": "./marshaler.h",
	"marshalerImplFile": "./marshaler.c"
}`
*/;
"""

@route("/")
def index():
	return static_file("index.html", ".")

@route("/upload")
def upload():
	return static_file("upload.html", ".")

@post("/generate")
def generate():
	zip = BytesIO()
	first_filename: Optional[str] = None
	with tempfile.TemporaryDirectory() as d:
		for file in request.files.values():
			file: FileUpload
			filename: str = file.filename
			if not filename.endswith(".h"):
				response.status = 400
				return f"Not a header file: {filename}"
			if first_filename is None:
				first_filename = file.filename
			file.save(d)
		if first_filename is None:
			response.status = 400
			return "You must provide a file"

		with open(f"{d}/{first_filename}", "r") as f:
			header_content = f.read()

		with open(f"{d}/{first_filename}", "w") as f:
			f.write(f"{CONFIG}\n{header_content}")

		result = subprocess.run(
			["cmarshal", "-DCMARSHAL_GENERATE", f"./{first_filename}"],
			cwd=d
		)

		if result.returncode != 0:
			response.status = 500
			return "Unable to generate"

		with zipfile.ZipFile(zip, "x") as z:
			z.write(f"{d}/marshaler.c", "marshaler.c")
			z.write(f"{d}/marshaler.h", "marshaler.h")
			z.write(f"{d}/unmarshaler.c", "unmarshaler.c")
			z.write(f"{d}/unmarshaler.h", "unmarshaler.h")
		zip.seek(0)

	response.content_type = "application/zip"
	response.set_header("Content-Disposition", f'attachment; filename="{first_filename}.zip"')
	return zip

run(host="0.0.0.0", port=int(os.getenv("PORT")))
