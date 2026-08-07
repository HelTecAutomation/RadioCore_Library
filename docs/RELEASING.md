# Releasing RadioCore_Kit

RadioCore_Kit uses Semantic Versioning. The version in `library.properties` is
the release version and each release tag must use the matching `vX.Y.Z` form.

## Release procedure

1. Update `version` in `library.properties` and prepare the GitHub release notes.
2. Run Arduino Lint with strict compliance and the applicable Library Manager
   mode. Use `submit` until the library is accepted into the registry, then use
   `update` for all later releases.
3. Confirm `git diff --check` passes and the tag name without its leading `v`
   exactly matches the version in `library.properties`.
4. Merge the verified release commit to `main`.
5. Create and push an annotated `vX.Y.Z` tag on that commit, then create a
   non-draft, non-prerelease GitHub Release from the tag. Use GitHub's generated
   source archives; do not attach a custom library ZIP.
6. Check the Arduino Library Manager indexer logs for the repository.

The release gate is static only. It does not compile the examples or prove
behavior on RC32, RCC6, RC52, or attached hardware. Release notes must state
this limitation.

Before the first version is accepted into the Library Manager index, a rejected
tag may be removed and recreated after fixing the release. Once a version has
entered the index, never move or recreate its tag; publish a new patch version
instead.
