#!/usr/bin/env python
#
# Utility to transcode the Zeitgeist ontology from Python code
# to a CPP header file defining a collection of definitions for the
# members of the ontology.
#

import sys
import getopt
import zeitgeist.datamodel as dm

def symbolname (symbol):
	try:
		ns, name = symbol[symbol.rfind("/")+1:].split("#")
		return "%s%s" % (ns.upper(), name)
	except Exception, e:
		return symbol.uri.rpartition("#")[2].upper()

def buildCdoc(name, symbol, docprefix=""):
	"""
	Builds a C-style docstring
	"""
	# Strip trailing ``(Display name: 'ReceiveEvent')`` text
	doc = symbol.__doc__
	doc = docprefix + doc
	doc = doc[:doc.find("``") - 1]

	# Fix link to external ontology
	doc = doc.replace(symbol.uri,
	                  '<ulink url="%s">%s</ulink>' % (symbol.uri, symbol.uri.replace("#", "&num;")))

	# Convert docstring to doxygen style C comment
	doc = doc.replace("\n", "\n *")
	doc = "/**\n * %s\n *\n * %s\n */" % (name, doc)
	return doc

def dumpFile(namespace, fields):
	# Print local namespace
	print "namespace %s\n{\n\n" % (namespace)

	zgitems = []
	sditems = []

	for item in fields:
		try:
			name = symbolname (item)
			doc = buildCdoc(name, item, docprefix="Macro defining the %s type " % (namespace))
			stmt = '%s\nconst char %s[] = "%s";' % (doc, name, item.uri)
			if "zeitgeist-project" in (item):
				zgitems.append(stmt)
			elif "semanticdesktop" in (item):
				sditems.append(stmt)
		except Exception, e:
			print >> sys.stderr, "Failed to convert %s: %s" % (item, e)

	# Sort both lists
	zgitems.sort()
	sditems.sort()

	if len(zgitems):
		print "namespace Event\n{\n\n"

		for item in zgitems:
			print item
			print ""

		print "\n};\n"

	if len(sditems):
		print "namespace Subject\n{\n\n"

		for item in sditems:
			print item
			print ""

		print "\n};\n"

	# Close local namespace
	print "\n};\n"

def printHeader(opts):
	header = (
		"/*\n"
		" * This file was generated by onto2cpp tool.\n"
		" * Command line was: onto2cpp %s\n\n"
		" * This is an auto-generated file.\n"
		" * Do not edit! All changes made to it will be lost.\n"
		" */\n\n" % (opts))

	print header

def main(argv):
	try:
		opts, args = getopt.getopt(argv, "o:", ["onto="])
	except getopt.GetoptError:
		print "Usage: onto2cpp -o [Interpretation:Manifestation]"
		sys.exit(2)

	printHeader(opts)

	fields = []

	for opt, arg in opts:
		if opt in ("-o", "--onto"):
			if arg == "Interpretation":
				fields = dm.Interpretation.get_all_children()
			elif arg == "Manifestation":
				fields = dm.Manifestation.get_all_children()

	# Print header guard.
	guard = "QTZEITGEIST_%s_H" % (arg.upper())

	print "#ifndef %s" % (guard)
	print "#define %s\n\n" % (guard)

	# Print Global namespace
	print "namespace QtZeitgeist\n{\n\n"

	# Dump definitions to the stdout
	dumpFile(arg, fields)

	# Close Global namespace
	print "\n};\n"

	# Close header guard
	print "#endif // %s" % (guard)


if __name__ == "__main__":
	main(sys.argv[1:])
