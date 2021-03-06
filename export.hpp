#ifndef SLAW_EXPORT_H
#define SLAW_EXPORT_H

#define EXPORT(name) __attribute__((export_name(name)))
#define IMPORT(name) extern "C" __attribute__( \
	(import_module("env"), import_name(name)))

#endif