/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express oqr
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU Public License (the "GPL"), in which case the
 * provisions of the GPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only
 * under the terms of the GPL and not to allow others to use your
 * version of this file under the NPL, indicate your decision by
 * deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL.  If you do not delete
 * the provisions above, a recipient may use your version of this
 * file under either the NPL or the GPL.
 */

#ifndef jsscript_h___
#define jsscript_h___
/*
 * JS script descriptor.
 */
#include "jsatom.h"
#include "jsprvtd.h"

JS_BEGIN_EXTERN_C

/*
 * Exception handling runtime information.
 *
 * All fields except length are code offsets, relative to the beginning of
 * the script.  If script->trynotes is not null, it points to a vector of
 * these structs terminated by one with catchStart == 0.
 */
struct JSTryNote {
    ptrdiff_t    start;         /* start of try statement */
    ptrdiff_t    length;        /* count of try statement bytecodes */
    ptrdiff_t    catchStart;    /* start of catch block (0 if end) */
};

struct JSScript {
    jsbytecode   *code;         /* bytecodes and their immediate operands */
    uint32       length;        /* length of code vector */
    jsbytecode   *main;         /* main entry point, after predef'ing prolog */
    JSVersion    version;       /* JS version under which script was compiled */
    JSAtomMap    atomMap;       /* maps immediate index to literal struct */
    const char   *filename;     /* source filename or null */
    uintN        lineno;        /* base line number of script */
    uintN        depth;         /* maximum stack depth in slots */
    jssrcnote    *notes;        /* line number and other decompiling data */
    JSTryNote    *trynotes;     /* exception table for this script */
    JSPrincipals *principals;   /* principals for this script */
    JSObject     *object;       /* optional Script-class object wrapper */
};

#define JSSCRIPT_FIND_CATCH_START(script, pc, catchpc)                        \
    JS_BEGIN_MACRO                                                            \
        JSTryNote *tn_ = (script)->trynotes;                                  \
        jsbytecode *catchpc_ = NULL;                                          \
        if (tn_) {                                                            \
            ptrdiff_t offset_ = PTRDIFF(pc, (script)->main, jsbytecode);      \
            while (JS_UPTRDIFF(offset_, tn_->start) >= (jsuword)tn_->length)  \
                tn_++;                                                        \
            if (tn_->catchStart)                                              \
                catchpc_ = (script)->main + tn_->catchStart;                  \
        }                                                                     \
        catchpc = catchpc_;                                                   \
    JS_END_MACRO

extern JS_FRIEND_DATA(JSClass) js_ScriptClass;

extern JSObject *
js_InitScriptClass(JSContext *cx, JSObject *obj);

/*
 * Three successively less primitive ways to make a new JSScript.  The first
 * two do *not* call a non-null cx->runtime->newScriptHook -- only the last,
 * js_NewScriptFromCG, calls this optional debugger hook.
 *
 * The js_NewScript function can't know whether the script it creates belongs
 * to a function, or is top-level or eval code, but the debugger wants access
 * to the newly made script's function, if any -- so callers of js_NewScript
 * are responsible for notifying the debugger after successfully creating any
 * kind (function or other) of new JSScript.
 */
extern JSScript *
js_NewScript(JSContext *cx, uint32 length);

extern JSScript *
js_NewScriptFromParams(JSContext *cx, jsbytecode *code, uint32 length,
		       jsbytecode *prolog, uint32 prologLength,
		       const char *filename, uintN lineno, uintN depth,
		       jssrcnote *notes, JSTryNote *trynotes,
		       JSPrincipals *principals);

extern JS_FRIEND_API(JSScript *)
js_NewScriptFromCG(JSContext *cx, JSCodeGenerator *cg, JSFunction *fun);

/*
 * New-script-hook calling is factored from js_NewScriptFromCG so that it
 * and callers of js_XDRScript can share this code.  In the case of callers
 * of js_XDRScript, the hook should be invoked only after successful decode
 * of any owning function (the fun parameter) or script object (null fun).
 */
extern JS_FRIEND_API(void)
js_CallNewScriptHook(JSContext *cx, JSScript *script, JSFunction *fun);

extern void
js_DestroyScript(JSContext *cx, JSScript *script);

extern void
js_MarkScript(JSContext *cx, JSScript *script, void *arg);

extern jssrcnote *
js_GetSrcNote(JSScript *script, jsbytecode *pc);

extern uintN
js_PCToLineNumber(JSScript *script, jsbytecode *pc);

extern jsbytecode *
js_LineNumberToPC(JSScript *script, uintN lineno);

extern uintN
js_GetScriptLineExtent(JSScript *script);

/*
 * If magic is non-null, js_XDRScript succeeds on magic number mismatch but
 * returns false in *magic; it reflects a match via a true *magic out param.
 * If magic is null, js_XDRScript returns false on bad magic number errors,
 * which it reports.
 *
 * NB: callers must call js_CallNewScriptHook after successful JSXDR_DECODE
 * and subsequent set-up of owning function or script object, if any.
 */
extern JSBool
js_XDRScript(JSXDRState *xdr, JSScript **scriptp, JSBool *magic);

JS_END_EXTERN_C

#endif /* jsscript_h___ */