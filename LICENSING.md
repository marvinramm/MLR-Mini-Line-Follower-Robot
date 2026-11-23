# Licensing

**© 2024 Marvin Ramm** – https://github.com/marvinramm/MLR-Mini-Line-Follower-Robot

The MLR is open source. You may use, modify, build, share and sell it, including commercially, **as long as you credit the author**.

| Part | License | File |
|---|---|---|
| Firmware (`firmware/`), except the files below | [MIT](https://opensource.org/license/mit) | [`LICENSE`](LICENSE) |
| Hardware design files, images and documentation (`hardware/`, `docs/`, README, release files) | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) | [`hardware/LICENSE`](hardware/LICENSE) |

## Third-party code

`firmware/KeepAngle/Madgwick.cpp` and `Madgwick.h` implement Madgwick's IMU/AHRS filter (S. O. H. Madgwick, x-io Technologies), taken from the examples of the [FastIMU](https://github.com/LiquidCGS/FastIMU) library. According to their header, they are provided under the GNU General Public License. They keep that license, and a compiled `KeepAngle` firmware that contains them is subject to the GPL as a whole. The MIT license applies to all other files in `firmware/`.

## Attribution

Both licenses require attribution:

- **MIT:** keep the copyright notice and the license text in all copies of the code.
- **CC BY 4.0:** credit the author wherever you use or share the hardware or the documentation, e.g. in your README, on the PCB silkscreen, or in your documentation or publication. Please use this text:

  > Based on "MLR – Mini Line Follower Robot" by Marvin Ramm – https://github.com/marvinramm/MLR-Mini-Line-Follower-Robot – licensed under CC BY 4.0.

  Also state whether you made changes.

I'd love to hear about it if you build one (GitHub issue or e-mail to marvin@ramm-eng.de). This is a request, not a license condition.
