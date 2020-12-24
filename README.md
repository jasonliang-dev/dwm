# dwm

## Patches

- [fullgaps](https://dwm.suckless.org/patches/fullgaps/)

Plus, a small patch was made to run a command on startup:

```diff
// config.h
+ static const char startup[] = "$HOME/scripts/startup.sh";

// dwm.c
int
main(int argc, char *argv[])
{
	// ...

	scan();
+	int exitcode = system(startup);
+	(void)exitcode;
	run();
	cleanup();

	// ...
}
```
