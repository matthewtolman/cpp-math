const Builder = @import("std").build.Builder;
const std = @import("std");

pub fn build(b: *std.build.Builder) void {
	const target = b.standardTargetOptions(.{});
	const mode = b.standardReleaseOptions();
	
	const mt_maths = b.addStaticLibrary("mt-maths", null);
	mt_maths.setTarget(target);
	mt_maths.setBuildMode(mode);
	mt_maths.linkLibCpp();
	//mt_maths.force_pic = true;
	mt_maths.addCSourceFiles(&.{
		"src/mtmath_c.cpp",
		"src/impl/rational.cpp",
		"src/impl/big_int.cpp",
		"src/impl/byte_array.cpp",
	}, &.{
		"-std=c++20",
		"-Wall",
	});

	const mt_maths_tests = b.addExecutable("mt-maths-tests", null);
	mt_maths_tests.setTarget(target);
	mt_maths_tests.setBuildMode(mode);
	mt_maths_tests.linkLibCpp();
	mt_maths_tests.linkLibrary(mt_maths);
	//mt_maths_tests.force_pic = true;
	mt_maths_tests.addCSourceFiles(&.{
		"tests/main.cpp",
		"tests/rationals.cpp",
		"tests/big_int.cpp",
		"tests/byte_array.cpp",
		"tests/c_bindings/big_int.cpp",
	}, &.{
		"-std=c++20",
		"-Wall",
		"-Isrc/",
	});

	const test_cmd = mt_maths_tests.run();
	const test_step = b.step("test", "Run tests");
	test_step.dependOn(&test_cmd.step);

	b.default_step.dependOn(&mt_maths_tests.step);
	b.installArtifact(mt_maths);
	b.installArtifact(mt_maths_tests);
}

