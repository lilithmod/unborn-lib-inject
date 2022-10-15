import Process from "./Process";

const { __getRunningProcesses, __getProcessByWindowClass, __injectSharedLibrary } = require("bindings")("inject.node");

/**
 * Get a full list of running processes
 * @returns {Array<Process>}
 */
function getRunningProcesses(): Process[] {
    return __getRunningProcesses();
}

/**
 * Finds a process based off it's window's class
 * @param windowClass {string} the window class to search for
 * @returns {Process | undefined}
 */
function getProcessByWindowClass(windowClass: string): Process | undefined {
    return __getProcessByWindowClass(windowClass);
}

/**
 * Inject a shared library into a different process
 * @param process {Process} process to inject into
 * @param libraryPath {string} path to shared library
 * @returns {boolean}
 */
function injectSharedLibrary(process: Process, libraryPath: string): boolean {
    return __injectSharedLibrary(process, libraryPath);
}

export {
    getRunningProcesses,
    getProcessByWindowClass,
    injectSharedLibrary,
    Process
};