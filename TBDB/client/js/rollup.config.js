const dts = require("rollup-plugin-dts").default;
const esbuild = require("rollup-plugin-esbuild").default;
const commonjs = require("@rollup/plugin-commonjs").default;
const packageJson = require("./package.json");

const name = packageJson.main.replace(/\.js$/, "");

const bundle = (config) => ({
  ...config,
  input: "./src/index.ts",
  external: (id) => !/^[./]/.test(id),
});

module.exports = [
  bundle({
    plugins: [
      esbuild(),
      commonjs({ esmExternals: true, requireReturnsDefault: true }),
    ],
    output: [
      {
        file: `${name}.mjs`,
        format: "es",
        sourcemap: false,
        exports: "named",
      },
    ],
  }),
  bundle({
    plugins: [
      esbuild(),
      commonjs({ esmExternals: true, requireReturnsDefault: true }),
    ],
    output: [
      {
        file: `${name}.js`,
        format: "cjs",
        sourcemap: false,
        exports: "named",
      },
    ],
  }),
  bundle({
    plugins: [dts()],
    output: {
      file: `${name}.d.ts`,
      format: "es",
    },
  }),
];
