export const logger = (log: boolean) => ({
  info(...args: any[]) {
    log && console.log("TOBSDB:INFO", ...args);
  },
  debug(...args: any[]) {
    log && console.log("TOBSDB:DEBUG", ...args);
  },
  error(...args: any[]) {
    log && console.log("TOBSDB:ERROR", ...args);
  },
  warn(...args: any[]) {
    log && console.log("TOBSDB:WARN", ...args);
  },
});
