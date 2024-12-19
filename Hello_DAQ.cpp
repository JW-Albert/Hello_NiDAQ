#include <iostream>
#include "NIDAQmx.h"

using namespace std;

#define DAQmxErrChk(functionCall) if (DAQmxFailed(error = (functionCall))) {DAQmxGetExtendedErrorInfo(errBuff, 2048); cerr << "DAQmx Error: " << errBuff << endl; goto Error;}

int main() {
    int32 error = 0; // 儲存錯誤代碼
    TaskHandle taskHandle = 0; // 定義任務控制代碼
    char errBuff[2048] = { 0 }; // 儲存錯誤訊息的緩衝區
    float64 data[2000]; // 緩衝區用於存儲讀取的數據 (2000 是為了讀取兩個通道的數據)
    int32 read; // 實際讀取的數據點數量

    try {
        // 創建任務
        DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

        // 創建 NI 9234 模組的電壓輸入通道，讀取 ai0 和 ai1
        DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "cDAQ1Mod1/ai0,cDAQ1Mod1/ai1", "", DAQmx_Val_Cfg_Default, -5.0, 5.0, DAQmx_Val_Volts, NULL));

        // 配置採樣時序，採樣率為 1000 Hz
        DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 1000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1000));

        // 啟動任務
        DAQmxErrChk(DAQmxStartTask(taskHandle));

        cout << "正在從 NI 9234 擷取電壓值... 按 Ctrl+C 終止程式。" << endl;

        while (true) {
            // 從通道讀取數據
            DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1000, 10.0, DAQmx_Val_GroupByScanNumber, data, 2000, &read, NULL));

            // 打印讀取到的數據
            cout << "讀取到 " << read << " 筆數據: " << endl;
            for (int i = 0; i < read; ++i) {
                cout << "ai0: " << data[i * 2] << ", ai1: " << data[i * 2 + 1] << endl;
            }
        }
    }
    catch (...) {
        cerr << "發生意外錯誤。" << endl;
        goto Error;
    }

Error:
    if (DAQmxFailed(error)) {
        cerr << "DAQmx 錯誤: " << errBuff << endl;
    }
    if (taskHandle != 0) {
        DAQmxStopTask(taskHandle); // 停止任務
        DAQmxClearTask(taskHandle); // 清除任務
    }
    return 0;
}
