/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 */

/*
 * This pragma unpacks the jnilib into a temp dir, and calls system.load
 */

%pragma(java) jniclasscode=%{
static {
	try {
		System.loadLibrary("example");
	} catch (UnsatisfiedLinkError e) {
		System.err.println("Native code library failed to load. \n" + e);
		System.exit(1);
	}
	}
%}