import os
import subprocess
import sys
from pathlib import Path


class VulkanNotInstalledError(Exception):
    pass


def main() -> None:
    if os.getenv("VULKAN_SDK") is None:
        raise VulkanNotInstalledError(
            "Vulkan is not installed, or 'VULKAN_SDK' environment variable is not set"
        )
    vulkan_bin_folder = Path(os.getenv("VULKAN_SDK")) / "Bin"
    os.environ["PATH"] += f"{os.pathsep}{vulkan_bin_folder}"

    shader_folder = Path(__file__).parent.parent / "shaders"
    subprocess.run(
        [
            "glslc.exe",
            str(shader_folder / "shader2D.vert"),
            "-o",
            str(shader_folder / "bin" / "shader2d.vert.spv"),
        ],
        check=True,
    )
    subprocess.run(
        [
            "glslc.exe",
            str(shader_folder / "shader2D.frag"),
            "-o",
            str(shader_folder / "bin" / "shader2d.frag.spv"),
        ],
        check=True,
    )


if __name__ == "__main__":
    main()
