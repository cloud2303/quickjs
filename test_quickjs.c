#include "quickjs.h"
#include <stdio.h>
#include <string.h>

static JSValue js_print(JSContext *ctx, JSValueConst this_val,
                        int argc, JSValueConst *argv) {
    int i;
    const char *str;

    // 遍历传入的参数并输出
    for (i = 0; i < argc; i++) {
        if (i != 0)
            putchar(' ');  // 参数之间用空格分隔
        // 将 JSValue 转换为 C 字符串
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;  // 如果转换失败，返回异常
        fputs(str, stdout);  // 输出字符串
        JS_FreeCString(ctx, str);  // 释放 JS_ToCString 返回的 C 字符串
    }
    putchar('\n');
    return JS_UNDEFINED;  // 返回 undefined
}

void init_console(JSContext *ctx)
{
    // 获取全局对象
    JSValue global_obj = JS_GetGlobalObject(ctx);
    // 创建 console 对象
    JSValue console = JS_NewObject(ctx);

    // 将 js_print 函数作为 log 方法挂载到 console 对象上
    JS_SetPropertyStr(ctx, console, "log",
                      JS_NewCFunction(ctx, js_print, "log", 1));

    // 将 console 对象挂载到全局对象的 console 属性上
    JS_SetPropertyStr(ctx, global_obj, "console", console);

    // 释放全局对象，console 不需要释放，它的生命周期由全局对象管理
    JS_FreeValue(ctx, global_obj);
}
int main()
{
    // 创建 QuickJS 运行时和上下文
    JSRuntime *runtime = JS_NewRuntime();
    JSContext *context = JS_NewContext(runtime);

    if (!context)
    {
        printf("无法创建JS上下文\n");
        return 1;
    }
    init_console(context);
    // 执行一段简单的 JavaScript 代码
    const char *script = "let x = 10; x * 2;";
    JSValue result = JS_Eval(context, script, strlen(script), "<input>", JS_EVAL_TYPE_GLOBAL);

    // 检查执行结果
    if (JS_IsException(result))
    {
        printf("JavaScript 执行出现异常\n");
    }
    else
    {
        // 将结果转换为 C 字符串并打印
        const char *str = JS_ToCString(context, result);
        printf("JavaScript 执行结果: %s\n", str);
        JS_FreeCString(context, str);
    }

    // 打印hello world
    const char *script2 = "console.log('hello 你好');";
    JSValue result2 = JS_Eval(context, script2, strlen(script2), "<input>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(result2))
    {
        printf("JavaScript 执行出现异常\n");
    }

    // 释放资源
    JS_FreeValue(context, result);
    JS_FreeContext(context);
    JS_FreeRuntime(runtime);

    return 0;
}