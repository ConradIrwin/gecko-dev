// |jit-test| error:InternalError

// This test is temporarily disabled in GGC builds (bug 950932).
if (getBuildConfiguration()['generational-gc'])
    (function f() { f(); })();

// Binary: cache/js-dbg-64-67bf9a4a1f77-linux
// Flags: --ion-eager
//

var actual = '';
test();
function test()
{
  a = {x: 1};
  b = {__proto__: a};
    print(actual += test(1,2,3,4));
}
