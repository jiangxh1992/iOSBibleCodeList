2.1.9 Objective-C的数组或字典中，添加nil对象会有什么问题？

/* 1.糖衣语法 */
NSArray *array = @[@1, @2, @3, nil]; // 错误，不可有nil，会编译不通过：void*不是Objective-C对象
NSDictionary *dic = @{
                      @"KEY":@"VALUE",
                      @"KEY1":@"VALUE1",
                      @"KEY2":nil
                      }; // 语法就是错误的，编译不通过
/* 2.原用法 */
NSMutableArray *mulArray = [[NSMutableArray alloc] initWithObjects:@1, @2, @3, nil]; // 正确
NSMutableDictionary *mulDic = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
                               @"VALUE", @"KEY",
                               @"VALUE1", @"KEY1", nil]; // 正确
/* 下面添加nil都会编译警告，运行起来会崩溃 */
[mulArray addObject:nil];
[mulDic setObject:nil forKey:@"KEY2"];


3.1
常见面试笔试真题9：如何只通过属性特质的参数来实现公有的getter方法和私有的setter方法？
手动实现代码：

/* Person.h头文件 */
@interface Person : NSObject {
@private
    NSString *name;
}
/* 声明公有的getter方法 */
- (NSString *)name;
@end

/* Person.m实现文件 */
#import "Person.h"
/* continue 私有声明区域 */
@interface Person()
/* 在.m文件的continue区域声明私有setter方法，通常私有方法不需要声明，可以省略 */
- (void)setName:(NSString *)newName;
@end

/* implementation实现区域 */
@implementation Person
/* 公有的getter方法实现 */
- (NSString *)name {
    /* 注意这里直接返回实例变量，如果使用self.name，那么相当于getter方法调用自身，此时会造成死循环 */
    return name;
}

/ * 私有的setter方法实现 */
- (void) setName:(NSString *)newName {
    /* 注意这里直接给实例变量赋值，如果使用self.name，那么相当于setter方法调用自身，此时会造成死循环 */
    name = newName;
}
@end


属性读写语义法代码：

/* Person.h头文件 */
@interface Person : NSObject
/* 使用readonly，让编译器只合成公有getter方法 */
@property (nonatomic, readonly, copy) NSString *name;
@end

/* Person.m实现文件 */
/* continue 私有声明区域 */
@interface Person()
/* 让编译器再合成私有setter方法，其中readwrite可以省略，因为默认就是readwrite */
@property (nonatomic, readwrite, copy) NSString *name;
@end

/* implementation实现区域 */
@implementation Person
/* 测试 */
- (void)test {
    /* 下面两条语句等效，都是调用setter方法，但注意setter方法是私有的，只能在此处调用，在外部无法调用 */
    self.name = @"name";
    [self setName:@"name"];
}
@end

3.2.1 什么叫多态
Animal父类：

// Animal.h
@interface Animal : NSObject
/* 父类接口，动物叫声 */
- (void)shout;
@end
// Animal.m
#import "Animal.h"
@implementation Animal
/* 父类接口的默认实现，无语 */
- (void)shout {
    NSLog(@"... ...");
}
@end


Dog子类：

// Dog.h
#import "Animal.h"
@interface Dog : Animal
/ * 重写父类接口，狗叫声 */
- (void)shout;
@end

// Dog.m
#import "Dog.h"
@implementation Dog
/ * 重写父类接口，狗叫声 */
- (void)shout {
    NSLog(@"汪汪汪，汪汪汪");
}
@end

Cat子类：

// Cat.h
#import "Animal.h"
@interface Cat : Animal
/ * 重写父类接口，猫叫声 */
- (void)shout;
@end

// Cat.m
#import "Cat.h"
@implementation Cat
/ * 重写父类接口，猫叫声 */
- (void)shout {
    NSLog(@"喵喵喵，喵喵喵");
}
@end


多态性测试：

/* 1. 指向Animal父类对象的Animal父类指针 */
Animal *p_animal4animal = [[Animal alloc] init];
/* 2. 指向Dog子类对象的Animal父类指针 */
Animal *p_animal4dog = [[Dog alloc] init];
/* 3. 指向Cat子类对象的Animal父类指针 */
Animal *p_animal4cat = [[Cat alloc] init];
/* 向指向不同对象的父类指针发送相同的消息，期望得到各自不同的结果，实现多态 */
[p_animal4animal shout]; // 打印结果：... ...
[p_animal4dog shout];    // 打印结果：汪汪汪，汪汪汪
[p_animal4cat shout];    // 打印结果：喵喵喵，喵喵喵


3.2.3    Objective-C和Swift中有重载吗？

- (void)test:(int)one;

- (void)test:(int)one andTwo:(int)two;
//- (void)test:(int)one;
//- (int)test:(float)one; // Duplicate declaration of method ‘test’

- (void)test:(int)one {
    NSLog(@"one parameter!");
}
- (void)test:(int)one andTwo:(int)two {
    NSLog(@"two parameters!");
}

[self test:1];          // output:one parameter!
[self test:1 andTwo:2]; // output:two parameter!

3.3.2
常见面试笔试真题：对于语句NSString* testObject = [[NSData alloc] init];，testObject在编译时和运行时分别是什么类型的对象？

/* 1.编译时编译器认为testObject是一个NSString对象，这里赋给它一个NSData对象编译器给出黄色类型错误警告，但运行时却是指向一个NSData对象 */
NSString* testObject = [[NSData alloc] init];
/* 2.编译器认为testObject是NSString对象，所以允许其调用NSString的方法，这里编译通过无警告和错误 */
[testObject stringByAppendingString:@"string"];
/* 3.但不允许其调用NSData的方法，下面这里编译不通过给出红色报错 */
[testObject base64EncodedDataWithOptions:NSDataBase64Encoding64CharacterLineLength];

/* 1.id任意类型，编译器就不会把testObject在当成NSString对象了 */
id testObject = [[NSData alloc] init];
/* 2.调用NSData的方法编译通过 */
[testObject base64EncodedDataWithOptions:NSDataBase64Encoding64CharacterLineLength];
/* 3.调用NSString的方法编译也通过 */
[testObject stringByAppendingString:@"string"];


3.3.8    如何打印一个类中所有的实例变量？

/* 新建Person类，为其添加属性 */
#import <Foundation/Foundation.h>
@interface Person : NSObject
@property (nonatomic, strong) NSString *name;
@property (nonatomic, assign) NSInteger age;
@end
#import "Person.h"
@interface Person ()
@property (nonatomic, strong) NSString *sex;
@end
@implementation Person
- (instancetype)init {
    if (self = [super init]) {
        self.sex = @"male";
    }
    return self;
}
@end
/* 在控制器中获取其实例变量列表并打印 */
Person *aperson = [[Person alloc]init];
aperson.age = 20;
aperson.name = @"sam";

unsigned int count = 0;
/* 用一个字典装实例变量的名称和值 */
NSMutableDictionary *dict = [NSMutableDictionary dictionary];
Ivar *list = class_copyIvarList([aperson class], &count);
for (int i = 0; i<count; i++) {
    NSString *ivarName = [NSString stringWithUTF8String:ivar_getName(list[i])];
    id ivarValue = [aperson valueForKey:ivarName];
    if (ivarValue) {
        dict[ivarName] = ivarValue;
    }else {
        dict[ivarName] = @"";
    }
}
/* 打印 */
for (NSString *ivarName in dict.allKeys) {
    NSLog(@"ivarName:%@, ivarValue:%@",ivarName, dict[ivarName]);
}


3.3.9    如何使用runtime动态添加一个类？

/* 创建一个新类myClass */
Class myClass = objc_allocateClassPair([NSObject class], "myClass", 0);
/* 添加ivar */
//@encode(aType) : 返回该类型的C字符串
class_addIvar(myClass, "_address", sizeof(NSString *), log2(sizeof(NSString *)), @encode(NSString *));
class_addIvar(myClass, "_age", sizeof(NSUInteger), log2(sizeof(NSUInteger)), @encode(NSUInteger));
/* 注册类 */
objc_registerClassPair(myClass);
/* 创建实例 */
id object = [[myClass alloc]init];
/* 为ivar设置值 */
[object setValue:@"china" forKey:@"address"];
[object setValue:@20 forKey:@"age"];
NSLog(@"address = %@,age = %@",[object valueForKey:@"address"],[object valueForKey:@"age"]);
/* 当类或者它的子类的实例还存在，则不能调用objc_disposeClassPair方法 */
object = nil;
/* 销毁 */
objc_disposeClassPair(myClass);

3.3.12    如何理解消息转发机制？

1、动态方法解析

/* 在控制器中调用Person类的实例方法eat，但是eat方法没有实现 */
Person *aperson = [[Person alloc]init];
[aperson performSelector:@selector(eat)];
/* 在Person类中为eat动态添加方法的实现 */
+ (BOOL)resolveInstanceMethod:(SEL)sel {
    NSString *str = NSStringFromSelector(sel);
    if ([str isEqualToString:@"eat"]) {
        IMP imp = method_getImplementation(class_getInstanceMethod(self, @selector(breakfast)));
        class_addMethod(self, @selector(eat),imp , "");
    }
    return [super resolveInstanceMethod:sel];
}
- (void)breakfast {
    NSLog(@"我吃了早餐了");
}

2、备用接收者

/* 新建一个PersonHelper类 */
@implementation PersonHelper
@interface PersonHelper : NSObject
- (void)eat;
@end
- (void)eat {
    NSLog(@"别人帮我吃东西");
}
@end
/* 在Person.m中设置PersonHelper类的实例为属性 */
@interface Person ()
@property (nonatomic, strong) PersonHelper *helper;
@end
@implementation Person
- (instancetype)init {
    if (self = [super init]) {
        _helper = [[PersonHelper alloc]init];
    }
    return self;
}
/* 实现forwardingTargetForSelector:方法 */
- (id)forwardingTargetForSelector:(SEL)aSelector {
    NSString *str = NSStringFromSelector(aSelector);
    if ([str isEqualToString:@"eat"]) {
        return _helper;
    }
    return [super forwardingTargetForSelector:aSelector];
}
@end
/* 在控制器中执行eat方法 */
Person *aperson = [[Person alloc]init];
[aperson performSelector:@selector(eat)];

3、完整的消息转发机制

/* 在Person.m中实现完整的消息转发 */
- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    NSMethodSignature *signature = [super methodSignatureForSelector:aSelector];
    if (!signature && [_helper methodSignatureForSelector:aSelector]) {
        signature = [_helper methodSignatureForSelector:aSelector];
    }
    return signature;
}
- (void)forwardInvocation:(NSInvocation *)anInvocation {
    if ([_helper respondsToSelector:anInvocation.selector]) {
        [anInvocation invokeWithTarget:_helper];
    }
}

3.3.14    Objective-C有私有方法吗? 私有变量呢?

#import <Foundation/Foundation.h>
@interface Test : NSObject {
    /* 头文件中定义私有变量，默认为@protected */
@private
    NSString *major;
}
@end
#import "Test.h"
@interface Test() {
    /* 类扩展区域定义私有变量，默认就是@private */
    int age;
}
/* 类扩展区域定义私有属性 */
@property (nonatomic, copy) NSString *name;
/* 类扩展区域定义私有实例方法(可省略声明，类方法的作用主要就是提供对外接口的，所以一般不会定义为私有) */
- (void)test;
@end
@implementation Test
/* 私有实例方法 */
- (void)test {
    NSLog(@"这是个私有实例方法！");
}
@end


利用KVC和runtime暴力访问私有属性和变量：

#import "Test.h"
#import <objc/runtime.h>
...
Test *test = [[Test alloc] init];
/* 1.KVO暴力访问私有属性和私有变量 */
[test setValue:@"albert" forKeyPath:@"name"];
NSString *pname = [test valueForKey:@"name"];
[test setValue:@"mathmetics" forKeyPath:@"major"];
NSString *pmajor = [test valueForKey:@"major"];
NSLog(@"pname:%@ pmajor:%@",pname,pmajor);


#import "Test.h"
#import <objc/runtime.h>
...
Test *test = [[Test alloc] init];
/* 2.运行时暴力访问私有属性和私有变量（ARC） */
/* 获取实例变量列表 */
unsigned int count = 0;
Ivar *members = class_copyIvarList([Test class], &count);
/* 打印所有的变量名及其类型 */
for (int i = 0; i < count; i++) {
    const char *memberName = ivar_getName(members[i]);
    const char *memberType = ivar_getTypeEncoding(members[i]);
    NSLog(@"name:%s type:%s",memberName,memberType);
}
/* 访问私有属性和变量 */
Ivar varname = members[0];
Ivar varmajor = members[1];
object_setIvar(test, varname, @"albert");
object_setIvar(test, varmajor, @"mathmetics");
NSString *name = object_getIvar(test, varname);
NSString *major = object_getIvar(test, varmajor);
NSLog(@"pname:%@",name);
NSLog(@"pmajor:%@",major);
/* 要手动释放 */
free(members);

Runtime暴力访问对象私有方法：

#import "Test.h"
#import <objc/runtime.h>
...
Test *test = [[Test alloc] init];
/* 运行时暴力访问私有方法 */
/* 获取类对象方法列表 */
unsigned int count = 0;
Method *methods = class_copyMethodList([Test class], &count);
/* 获取第一个方法的方法名 */
SEL sel = method_getName(methods[0]);
/* 执行该方法 */
[test performSelector:sel];

3.4.4    为什么类别只能添加扩展方法而不能添加属性变量？

/* NSString+Category.h */
#import <Foundation/Foundation.h>

@interface NSString (Category)
/* 在类别中扩展属性 */
@property (nonatomic, copy) NSString *newString;
@end
/* NSString+Category.m */
#import "NSString+Category.h"
#import <objc/runtime.h>

@implementation NSString (Category)
* 运行时强行实现newString的getter和setter */
- (NSString *)newString {
    return objc_getAssociatedObject(self, @"newString");
}
- (void)setNewString:(NSString *)newString {
    objc_setAssociatedObject(self, @"newString", newString, OBJC_ASSOCIATION_COPY);
}
@end
/* main.m */
#import <Foundation/Foundation.h>
#import "NSString+Category.h"

int main(int argc, const char * argv[]) {
    NSString *string;
    /* 调用newString的setter方法 */
    string.newString = @"newString";
    /* 调用newString的getter方法 */
    NSLog(@"%@",string.newString);
    return 0;
}

3.5.1    Method Swizzling的应用场景有哪些？

1、替换类中某两个类方法或实例方法的实现

/* Test.h */
#import <Foundation/Foundation.h>
@interface Test : NSObject
/*定义两个公有实例方法*/
- (void)instanceMethod1;
- (void)instanceMethod2;
/* 定义两个公有类方法*/
+ (void)classMethod1;
+ (void)classMethod2;
@end
/* Test.m */
#import "Test.h"
#import <objc/runtime.h>
@implementation Test
/*实例方法的原实现*/
- (void)instanceMethod1 {
    NSLog(@"instanceMethod1...");
}
- (void)instanceMethod2 {
    NSLog(@"instanceMethod2...");
}
/* 类方法的原实现*/
+ (void)classMethod1 {
    NSLog(@"classMethod1...");
}
+ (void)classMethod2 {
    NSLog(@"classMethod2...");
}
@end
/* runtime代码写在类第一次被调用加载的时候(load方法有且只有一次会被调用)*/
+ (void)load {
    /* 1. 获取当前类名 */
    Class class = [self class];
    /* 2. 获取方法名(选择器) */
    SEL selInsMethod1 = @selector(instanceMethod1);
    SEL selInsMethod2 = @selector(instanceMethod2);
    SEL selClassMethod1 = @selector(classMethod1);
    SEL selClassMethod2 = @selector(classMethod2);
    /* 3. 根据方法名获取方法对象 */
    Method InsMethod1 = class_getInstanceMethod(class, selInsMethod1);
    Method InsMethod2 = class_getInstanceMethod(class, selInsMethod2);
    Method ClassMethod1 = class_getClassMethod(class, selClassMethod1);
    Method ClassMethod2 = class_getClassMethod(class, selClassMethod2);
    /* 4. 交换实例方法的实现和类方法的实现 */
    if (!InsMethod1 || !InsMethod2) {
        NSLog(@"实例方法实现运行时交换失败！");
        return;
    }
    /* 交换实例方法的实现 */
    method_exchangeImplementations(InsMethod1, InsMethod2);
    if (!ClassMethod1 || !ClassMethod2) {
        NSLog(@"类方法实现运行时交换失败！");
        return;
    }
    /* 交换类方法的实现 */
    method_exchangeImplementations(ClassMethod1, ClassMethod2);
}


#import "ViewController.h"
#import "Test.h"
@implementation ViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    /* 测试类方法调用 */
    [Test classMethod1];
    [Test classMethod2];
    Test *test = [[Test alloc] init];
    /* 测试实例方法调用 */
    [test instanceMethod1];
    [test instanceMethod2];
}
@end


2、重新设置类中某个方法的实现

/* 获取方法的实现 */
IMP impInsMethod1 = method_getImplementation(InsMethod1);
IMP impInsMethod2 = method_getImplementation(InsMethod2);
IMP impClassMethod1 = method_getImplementation(ClassMethod1);
IMP impClassMethod2 = method_getImplementation(ClassMethod2);
/* 重新设置方法的实现 */
/* 重新设置instanceMethod1的实现为instanceMethod2的实现 */
method_setImplementation(InsMethod1, impInsMethod2);
/* 重新设置instanceMethod2的实现为instanceMethod1的实现 */
method_setImplementation(InsMethod2, impInsMethod1);
/* 重新设置classMethod1的实现为classMethod2的实现 */
method_setImplementation(ClassMethod1, impClassMethod2);
/* 重新设置classMethod2的实现为classMethod1的实现 */
method_setImplementation(ClassMethod2, impClassMethod1);


3、替换类中某个方法的实现

/* 获取方法编码类型 */
const char* typeInsMethod1 = method_getTypeEncoding(InsMethod1);
const char* typeInsMethod2 = method_getTypeEncoding(InsMethod2);
const char* typeClassMethod1 = method_getTypeEncoding(ClassMethod1);
const char* typeClassMethod2 = method_getTypeEncoding(ClassMethod2);
/* 替换InsMethod1的实现为InsMethod2的实现 */
class_replaceMethod(class, selInsMethod1, impInsMethod2, typeInsMethod2);
/* 替换InsMethod2的实现为InsMethod1的实现 */
class_replaceMethod(class, selInsMethod2, impInsMethod1, typeInsMethod1);
/* 尝试替换类方法的实现 */
class_replaceMethod(class, selClassMethod1, impClassMethod2, typeClassMethod2);
class_replaceMethod(class, selClassMethod2, impClassMethod1, typeClassMethod1);


4、在运行时为类添加新的方法

/*  为类添加新的实例方法和类方法 */
SEL selNewInsMethod = @selector(newInsMethod);
BOOL isInsAdded = class_addMethod(class, selNewInsMethod, impInsMethod1, typeInsMethod1);
if (!isInsAdded) {
    NSLog(@"新实例方法添加失败！");
}

/* 测试运行时新添加实例方法调用 */
Test *test = [[Test alloc] init];
[test newInsMethod];


3.6.2    Swift和Objective-C如何互调？

1、在SwiftClass.swift中定义一个实例方法和一个类方法：
import UIKit
class SwiftClass: NSObject {
    func SwiftInstanceMethod() -> Void {
        print("swift instance method!");
    }
    class func SwiftClassMethod() -> Void {
        print("swift class method!");
    }
}
2、在OCClass类中调用SwiftClass：
/* OCClass.h */
#import <Foundation/Foundation.h>

@interface OCClass : NSObject
- (void)OCInstanceMethod;
+ (void)OCClassMethod;
@end

/* OCClass.m */
#import "OCClass.h"
#import "SwiftBridge-Swift.h" /* 引入swift类头文件 */
@implementation OCClass
- (void)OCInstanceMethod {
    /* 调用swift实例方法 */
    SwiftClass *swiftc = [[SwiftClass alloc] init];
    [swiftc SwiftInstanceMethod];
    NSLog(@"oc instance method!");
}
+ (void)OCClassMethod {
    /* 调用swift类方法 */
    [SwiftClass SwiftClassMethod];
    NSLog(@"oc class method!");
}
@end
3、在Bridging头文件引入Objective-C类头文件供swift调用：
/* SwiftBridge-Bridging-Header.h */
#import "OCClass.h"
4、在ViewController.swift中调用OCClass类：
/* ViewController.swift */
import UIKit
class ViewController: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
        let occ = OCClass()
        occ.ocInstanceMethod()
        OCClass.ocClassMethod()
    }
}


4.1.1  UITableViewCell的复用原理是怎么样的

测试代码：

/* 分区个数设置为1 */
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}
/ * 创建20个cell，保证覆盖并超出整个tableView */
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 20;
}
/* cell复用机制测试 */
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    /* 定义cell重用的静态标志符 */
    static NSString *cell_id = @"cell_id_demo";
    /* 计数用 */
    static int countNumber = 1;
    /* 优先使用可复用的cell */
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cell_id];
    /* 如果要复用的cell还没有创建，那么创建一个供之后复用 */
    if (cell == nil) {
        /* 新创建cell并使用cell_id复用符标记 */
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier: cell_id];
        /* 计数器标记新创建的cell */
        cell.textLabel.text = [NSString stringWithFormat:@"Cell%i", countNumber];
        /* 计数器递增 */
        countNumber++;
    }
    return cell;
}

4.1.3  一个tableView是否可以关联两个不同的datasource数据源


//合并数据用到的数据模型：
@interface Model : NSObject
@property (nonatomic,copy) NSString *name;  // 姓名
@property (nonatomic,copy) NSString *url;   // 图片
@end

//数据源缓冲器：
/* 数据源 */
@property (nonatomic, strong)NSArray *name_datasource;
@property (nonatomic, strong)NSArray *url_datasource;
@property (nonatomic, strong)NSMutableArray *datasource;


//处理多数据源：
/* 请求数据 */
- (void)request {
    /* 姓名数据源 */
    _name_datasource = @[@"张三", @"李四", @"小明", @"小李"];
    _url_datasource = @[@"male", @"male", @"male", @"male"];
    /* 合并数据源 */
    for (int i; i<_name_datasource.count; i++) {
        Model *model = [[Model alloc]init];
        model.name = _name_datasource[i];
        model.url = _url_datasource[i];
        [_datasource addObject:model];
    }
}

//数据对接：
/*  cell数据回调 */
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *identifier = @"identifier";
    /* 自制cell组件 */
    AccountCell *cell = [[AccountCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
    /** 多数据源分开对接：**/
    /* 头像 */
    [cell.avatar setImage:[UIImage imageNamed:_url_datasource[indexPath.row]]];
    /* 姓名 */
    cell.name.text = _name_datasource[indexPath.row];
    
    /* 或者：*/
    /** 数据源合并后对接 **/
    /* 取出对应数据模型 */
    Model *model = _datasource[indexPath.row];
    /* 头像 */
    [cell.avatar setImage:[UIImage imageNamed:model.url]];
    /* 姓名 */
    cell.name.text = model.name;
    return cell;
}

4.3.4  UIDevice如何获取设备信息

1．UIDevice

//测试示例代码如下：
/* 1.UIDevice */
UIDevice *device = [UIDevice currentDevice]; // 获取当前的device
/** 设备基本信息 **/
NSString *name = [device name];// @"iPhone 5S"
NSString *model = [device model];// @"iPhone"
NSString *localizedModel = [device localizedModel];// @"iPhone"
NSString *systemName = [device systemName];// @"iPhone OS"
NSString *systemVersion = [device systemVersion];// @"9.3.2"
/** 设备方向 **/
UIDeviceOrientation orientation = [device orientation]; // UIDeviceOrientationUnknown
/* 设备类型：iPad或iPhone（iPod）等 */
if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad|UIUserInterfaceIdiomPhone){}
/** 设备电量 **/
device.batteryMonitoringEnabled = YES;
float batteryLevel = [UIDevice currentDevice].batteryLevel; // 0~1.0对应电量0~100%，-1.0表示电量未知
device.batteryMonitoringEnabled = YES;
/** 设备ID **/
NSUUID *identifierForVendor = [device identifierForVendor];

2．NSBundle

//测试示例代码如下：
/* 2.NSBundle */
/* 获取当前设备的信息字典 */
NSDictionary *info_dic = [[NSBundle mainBundle] infoDictionary];
/* 本地化的信息字典 */
NSDictionary *localized_info_dic = [[NSBundle mainBundle] localizedInfoDictionary];
NSString *app_name = [info_dic objectForKey:@"CFBundleDisplayName"]; // 应用名称 @"Demo"
NSString *app_version = [info_dic objectForKey:@"CFBundleShortVersionString"]; // 应用版本 @"1.0"
NSString *app_build_version = [info_dic objectForKey:@"CFBundleVersion"]; // 应用build版本 @"1"

3．NSLocale

//测试示例代码如下：
/* 3.NSLocale */
/* 3.1 获取偏好语言 */
NSArray *languageArray = [NSLocale preferredLanguages];
NSString *language = [languageArray objectAtIndex:0]; // @"en_HK"
/* 3.2 获取本地化国家或地区代号 */
NSLocale *locale = [NSLocale currentLocale]; // 当前本地化对象
NSString *country = [locale localeIdentifier]; // @"en_HK"

5.2.3  NSNotification是同步还是异步

//下面示例验证默认通知是同步的。

/* 自定义消息的名称 */
#define MYNotificationTestName @"NSNotificationTestName"
/* 1.注册通知的观察者 */
[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(process) name: MYNotification TestName object:nil];
/* 2.发出通知给观察者 */
NSLog(@"即将发出通知！");
[[NSNotificationCenter defaultCenter] postNotificationName:MYNotificationTestName object:nil];
NSLog(@"发出通知处的下一条代码！");
/*3.处理收到的通知*/
- (void)process {
    sleep(10); // 假设处理需要10s
    NSLog(@"通知处理结束！");
}

//将通知的发送语句放到子线程：

NSLog(@"即将发出通知！");
dispatch_async(dispatch_get_global_queue(0, 0), ^{
    [[NSNotificationCenter defaultCenter]
     postNotificationName:MYNotificationTestName object:nil];
});
NSLog(@"发出通知处的下一条代码！");

//或者：

NSLog(@"即将发出通知！");
/* 将通知放到通知异步缓冲队列 */
NSNotification *notification = [NSNotification notificationWithName:MYNotificationTestName object:nil];
[[NSNotificationQueue defaultQueue] enqueueNotification:notification postingStyle:NSPostASAP];
NSLog(@"发出通知处的下一条代码！");

//将通知的处理放到子线程：

/* 处理收到的通知*/
- (void)process {
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        sleep(10); // 假设处理需要10s
        NSLog(@"通知处理结束！");
    });
}

5.3.1  什么是键值编码？键路径是什么? 什么是键值观察

//3．键值观察KVO：

/* 1.专业类模型：Major.h */
@interface Major : NSObject {
@private
    NSString *majorName; // 私有实例变量专业名称
}
@end
/* 2.学生类模型:Student.h */
@interface Student : NSObject {
@private
    NSString *name; // 私有实例变量姓名
}
@property (nonatomic, strong)Major *major; // 学生专业
/* 3.kvc和kvo之间基本使用方法 */
#import "Student.h"
#import "Major.h"
@interface ViewController ()
@property (nonatomic, strong)Student *student;
@end
@implementation ViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    /* 初始化学生Student对象 */
    _student = [[Student alloc] init];
    /* 初始化学生的专业Major对象 */
    _student.major = [[Major alloc] init];
    /* 1.set: 通过KVC设置Student对象的值(可以强行访问private变量) */
    [_student setValue:@"Sam" forKey:@"name"];
    [_student setValue:@"Computer Science" forKeyPath:@"major.majorName"];
    /* 2.get: 通过KVC读取Student对象的值(可以强行访问private变量) */
    NSLog(@"%@", [_student valueForKey:@"name"]);
    NSLog(@"%@", [_student valueForKeyPath:@"major.majorName"]);
    /*
     **3.kvo:添加当前控制器为键路径major.majorName的一个观察者，
     **如果major.majorName的值改变，那么会通知当前控制器从而
     **自动调用下面的observeValueForKeyPath方法，这里传递旧值和新值
     */
    [_student addObserver:self forKeyPath:@"major.majorName" options:NSKeyValueObservingOption Old | NSKeyValueObservingOptionNew context:nil];
    /* 3s后改变major.majorName的值 */
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_ get_main_queue(), ^{
        [_student setValue:@"Software Engineer" forKeyPath:@"major.majorName"];
    });
}
/* 监听keyPath值的变化 */
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValue ChangeKey,id> *)change context:(void *)context {
    if ([keyPath isEqual:@"major.majorName"]) {
        /* 获取变化前后的值并打印 */
        NSString *oldValue = [change objectForKey:NSKeyValueChangeOldKey];
        NSString *newValue = [change objectForKey:NSKeyValueChangeNewKey];
        NSLog(@"major.majorName value changed:oldValue:%@ newValue:%@", oldValue,newValue);
    }
    else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}
/* 移除观察者释放资源，防止资源泄漏 */
- (void)dealloc {
    [_student removeObserver:self forKeyPath:@"major.majorName"];
}
@end


5.3.3  如何运用KVO进行键值观察

//1．注册成为观察者

/* 创建一个需要被观察对象的类 */
#import <Foundation/Foundation.h>
@interface Person : NSObject
@property (nonatomic, strong) NSString *name;
@property (nonatomic, assign) NSInteger age;
@end

#import "Person.h"
@implementation Person
@end
/* 其次在控制器中实例化一个被观察者对象并将控制器对象注册成为观察者 */
/* options参数设置为NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld，表示同时观察新值和旧值 */
aperson = [[Person alloc]init];
[aperson addObserver:self forKeyPath:@"age" options:NSKeyValueObservingOptionNew|NSKeyValue ObservingOptionOld context:nil];

//2．设置需要被观察对象的属性

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    aperson.age = 20;
    [aperson setValue:@"sam" forKey:@"name"];
}

//3．在回调方法中处理变更通知
- (void)observeValueForKeyPath:(nullable NSString *)keyPath ofObject:(nullable id)object change: (nullable NSDictionary<NSKeyValueChangeKey, id> *)change context:(nullable void *)context;

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary< NSKeyValueChangeKey,id> *)change context:(void *)context {
    if ([keyPath isEqualToString:@"age"]) {
        NSLog(@"oldAge = %@,newAge = %@",change[NSKeyValueChangeOldKey],change [NSKeyValueChangeNewKey]);
    }else if ([keyPath isEqualToString:@"name"]) {
        NSLog(@"oldName = %@,newName = %@",change[NSKeyValueChangeOldKey],change [NSKeyValueChangeNewKey]);
    }
}

//4．移除观察者

- (void)dealloc {
    [aperson removeObserver:self forKeyPath:@"age"];
    [aperson removeObserver:self forKeyPath:@"name"];
}


5.3.4  如何使用KVO设置键值观察依赖键

/* 新建一个Student类，其中的preson属性是Person类的实例*/
#import <Foundation/Foundation.h>
#import "Person.h"
@interface Student : NSObject
@property (nonatomic, strong) NSString *infomation;
@property (nonatomic, strong) Person *preson;
@end

#import "Student.h"
@implementation Student
@end

//在Student.m文件中添加如下代码：

- (NSString *)infomation {
    return [NSString stringWithFormat:@"student_name = %@,student_age = %zd",self.preson.name, self.preson.age];
}

- (void)setInfomation:(NSString *)infomation {
    NSArray * array = [infomation componentsSeparatedByString:@"#"];
    [self.preson setName:[array objectAtIndex:0]];
    [self.preson setAge:[[array objectAtIndex:1] integerValue]];
}

+ (NSSet *)keyPathsForValuesAffectingInfomation {
    NSSet *set = [NSSet setWithObjects:@"person.age", @"person.name",nil];
    return set;
}

//现在已经建立了依赖关系，还需要在控制器中添加观察者和实现KVO的回调方法：

- (void)viewDidLoad {
    astudent = [[Student alloc]init];
    astudent.person = [[Person alloc]init];
    [astudent addObserver:self forKeyPath:@"infomation" options:NSKeyValueObservingOptionNew |NSKeyValueObservingOptionOld context:nil];
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    astudent.person.age = 20;
    astudent.person.name = @"jack";
}
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary <NSKeyValueChangeKey,id> *)change context:(void *)context {
    if ([keyPath isEqualToString:@"infomation"]) {
        NSLog(@"old_infomation : %@ new_infomation : %@",change[NSKeyValueChangeOldKey], change[NSKeyValueChangeNewKey]);
    }
}

//解除观察者：

- (void)dealloc {
    [astudent removeObserver:self forKeyPath:@"infomation"];
}


5.3.6  setValue:forKey:方法的底层实现是什么

//示例代码如下：

/* 定义一个Person类 */
#import "Person.h"
@interface Person () {
    NSString *_name;
    int _age;
}
@property (nonatomic,assign) int age;
@property (nonatomic, copy) NSString *address;
@end
@implementation Person
-(void)setName:(NSString *)name
{
    NSLog(@"%s",__func__);
    _name = name;
}
- (void)log {
    NSLog(@"age = %i  address = %@",_age,self.address);
}
/* 编辑如下测试代码：*/
Person *p = [Person new];
[p setValue:@"小强" forKey:@"name"];
[p setValue:@25 forKey:@"age"];
[p setValue:@"合肥市" forKey:@"address"];
[p log]；

6.1.2  什么是Layer层对象

//通过CALayer来实现一个UILabel的示例代码如下：

- (void)viewDidLoad {
    [super viewDidLoad];
    /* 创建一个字符承载视图 */
    UIView *textView = [[UIView alloc] initWithFrame:CGRectMake(50, 50, 200, 50)];
    CATextLayer *text = [CATextLayer layer];
    text.frame = textView.frame;
    text.string = @"CAText";
    /* 文字前景色和背景色 */
    text.foregroundColor = [UIColor whiteColor].CGColor;
    text.backgroundColor = [UIColor blackColor].CGColor;
    /* 文字超出视图边界裁剪 */
    text.wrapped = YES;
    /* 文字字体 */
    text.font = (__bridge CFTypeRef)[UIFont systemFontOfSize:30].fontName;
    /* 文字居中 */
    text.alignmentMode = kCAAlignmentCenter;
    /* 适应屏幕Retina分辨率，防止像素化导致模糊 */
    text.contentsScale = [[UIScreen mainScreen] scale];
    [textView.layer addSublayer:text];
    [self.view addSubview:textView];
}

//使用CAShapeLayer绘制一个圆形的实例代码如下：

- (void)viewDidLoad {
    [super viewDidLoad];
    /* 创建圆形路径 */
    UIBezierPath *path = [[UIBezierPath alloc] init];
    /* 起点要在圆心水平右侧半径长度处 */
    [path moveToPoint:CGPointMake(200, 100)];
    /* 添加圆形弧路径 */
    [path addArcWithCenter:CGPointMake(150, 100) radius:50 startAngle:0 endAngle:2*M_PI clockwise:YES];
    
    /* 创建图形层 */
    CAShapeLayer *layer = [CAShapeLayer layer];
    /* 路径线的颜色 */
    layer.strokeColor = [UIColor redColor].CGColor;
    /* 闭合图形填充色，这里设置透明 */
    layer.fillColor = [UIColor clearColor].CGColor;
    /* 线宽 */
    layer.lineWidth = 10;
    /* 线的样式：端点、交点 */
    layer.lineCap = kCALineCapRound;
    layer.lineJoin = kCALineJoinRound;
    /* 设置图形路径 */
    layer.path = path.CGPath;
    [self.view.layer addSublayer:layer];
}

//3种颜色从正方形左上角到右下角的渐变效果示例代码：

- (void)viewDidLoad {
    [super viewDidLoad];
    /* 创建layer承载视图 */
    UIView *containerView = [[UIView alloc] initWithFrame:CGRectMake(50, 50, 150, 150)];
    CAGradientLayer *layer = [CAGradientLayer layer];
    layer.frame = containerView.bounds;
    /* 依次设置渐变颜色数组 */
    layer.colors = @[(__bridge id)[UIColor greenColor].CGColor,(__bridge id)[UIColor yellowColor].CGColor,(__bridge id)[UIColor orangeColor].CGColor];
    /* 颜色从起点到终点按比例分段位置 */
    layer.locations = @[@0.0, @0.3, @0.5];
    /* 颜色渐变的起点和终点：(0,0)~(1,1)表示左上角到右下角 */
    layer.startPoint = CGPointMake(0, 0);
    layer.endPoint = CGPointMake(1, 1);
    [containerView.layer addSublayer:layer];
    [self.view addSubview:containerView];
}


6.1.3  如何使用CAShapeLayer绘制图层

/* 创建一个CAShapeLayer对象 */
CAShapeLayer *shaperLayer = [CAShapeLayer layer];
/* 设置线宽 */
shaperLayer.lineWidth = 2;
shaperLayer.lineCap = kCALineCapRound;
shaperLayer.lineJoin = kCALineJoinRound;
/* 设置描边颜色 */
shaperLayer.strokeColor = [UIColor redColor].CGColor;
/* 设置填充颜色 */
shaperLayer.fillColor = [UIColor whiteColor].CGColor;
/* 创建一个UIBezierPath对象 */
UIBezierPath *bezierPath = [UIBezierPath bezierPath];
[bezierPath moveToPoint:CGPointMake(160, 100)];
[bezierPath addLineToPoint:CGPointMake(100, 160)];
[bezierPath addLineToPoint:CGPointMake(100, 220)];
[bezierPath addLineToPoint:CGPointMake(160, 280)];
[bezierPath addLineToPoint:CGPointMake(220, 220)];
[bezierPath addLineToPoint:CGPointMake(220, 160)];
[bezierPath closePath];
/* 绘制 */
shaperLayer.path = bezierPath.CGPath;
[self.view.layer addSublayer:shaperLayer];


//使用UIBezierPath绘制一段圆弧，代码如下：

-(void)drawRect:(CGRect)rect{
    UIBezierPath *path = [UIBezierPath bezierPathWithArcCenter:CGPointMake(100,100) radius:100 startAngle:0 endAngle:M_PI_2 clockwise:YES];
    path.lineWidth = 5;
    path.lineCapStyle = kCGLineCapRound;
    path.lineJoinStyle = kCGLineCapRound;
    [path stroke];
}


6.1.4  iOS中如何实现为UIImageView添加圆角

//（1）“离屏渲染”实现圆角

/* 设置圆角半径 */
view.layer.cornerRadius = 5;
/* 将边界以外的区域遮盖住 */
view.layer.masksToBounds = YES;


//（2）“当前屏幕渲染”实现圆角

//为UIImage类扩展一个实例方法：

/* On-screen-renderring绘制UIImage矩形圆角 */
- (UIImage *)imageWithCornerRadius:(CGFloat)radius ofSize:(CGSize)size{
    /* 当前UIImage的可见绘制区域 */
    CGRect rect = (CGRect){0.f,0.f,size};
    /* 创建基于位图的上下文 */
    UIGraphicsBeginImageContextWithOptions(size,NO,UIScreen.mainScreen.scale);
    /* 在当前位图上下文添加圆角绘制路径 */
    CGContextAddPath(UIGraphicsGetCurrentContext(),[UIBezierPath bezierPathWithRoundedRect:rect cornerRadius:radius].CGPath);
    /* 当前绘制路径和原绘制路径相交得到最终裁剪绘制路径 */
    CGContextClip(UIGraphicsGetCurrentContext());
    /* 绘制 */
    [self drawInRect:rect];
    /* 取得裁剪后的image */
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    /* 关闭当前位图上下文 */
    UIGraphicsEndImageContext();
    return image;
}

//使用时，让实例化的UIImage对象调用上面的实例方法即可：

UIImageView *imageView = [[UIImageView alloc] initWithFrame:CGRectMake(10, 10, 100, 100)];
/* 创建并初始化UIImage */
UIImage *image = [UIImage imageNamed:@"icon"];
/* 添加圆角矩形 */
image = [image imageWithCornerRadius:50 ofSize:imageView.frame.size];
[imageView setImage:image];

6.1.6  如何理解anchorPoint和position的作用

/* 控制器代码如下 */
@interface Animation (){
    __weak IBOutlet UIImageView *_needle;
    BOOL _playing;//设置一个布尔值来控制下降或上升
}
@end
@implementation Animation
- (void)viewDidLoad {
    [super viewDidLoad];
}
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    /* 改变视图的锚点和position */
    /* 如果仅仅改变锚点，那么_needle视图将会下移 */
    _needle.layer.anchorPoint = CGPointMake(0.5, 0);
    _needle.layer.position = CGPointMake(_needle.superview.frame.size.width*0.5, 0);
    _needle.transform = CGAffineTransformRotate(_needle.transform, M_PI/12);
}
/* 单击屏幕，移动臂杆 */
/* 当然也可以添加一些动画，使得臂杆的动作更自然，但此处主要是为了演示anchorPoint和position的作用 */
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
if (!_playing) {
    _needle.transform = CGAffineTransformIdentity;
}else {
    _needle.transform = CGAffineTransformRotate(_needle.transform, M_PI/12);
}
_playing = !_playing;
}

6.1.8  如何使用mask属性实现图层蒙版功能

//示例代码如下：

- (void)viewDidLoad {
    [super viewDidLoad];
    /* 设置寄宿图 */
    self.view.layer.contents = (id)[UIImage imageNamed:@“海水”].CGImage;
    /* 设置寄宿图的显示模式，等价于UIView的contentMode */
    self.view.layer.contentsGravity = @"resizeAspect";
    /* 设置mask层 */
    UIImageView *searchImageView = [[UIImageView alloc]initWithFrame:CGRectMake(([UIScreen mainScreen].bounds.size.width - 150)*0.5, ([UIScreen mainScreen].bounds.size.height - 150)*0.5, 150, 150)];
    searchImageView.image = [UIImage imageNamed:@"马"];
    self.view.layer.mask = searchImageView.layer;
}


6.1.9  如何解决masksToBounds离屏渲染带来的性能损耗

//使用UIKit中对Core Graphics有一定封装的应用层类UIBezierPath，对图片进行重新剪切。示例代码如下：

UIImageView *imageView = [[UIImageView alloc]initWithFrame:CGRectMake(100, 100, 100, 100)];
imageView.image = [UIImage imageNamed:@"1"];
/* 开启上下文 开始对imageView进行画图 */
UIGraphicsBeginImageContextWithOptions(imageView.bounds.size, NO, 1.0);
/* 使用贝塞尔曲线画出一个圆形图 */
[[UIBezierPath bezierPathWithRoundedRect:imageView.bounds   cornerRadius:imageView.frame.size. width] addClip];
[imageView drawRect:imageView.bounds];
imageView.image = UIGraphicsGetImageFromCurrentImageContext();
/* 关闭上下文 结束画图 */
UIGraphicsEndImageContext();
[self.view addSubview:imageView];


6.2.3  隐式动画的实现原理是什么？如何禁用图层的隐式动画

//使用+setDisableActions临时取消隐式动画的代码如下：

- (void)viewDidLoad {
    [super viewDidLoad];
    _colorLayer = [CALayer layer];
    _colorLayer.bounds = CGRectMake(0, 0, 200, 200);
    _colorLayer.position = self.view.center;
    _colorLayer.backgroundColor = [UIColor redColor].CGColor;
    _colorLayer.delegate = self;
    [self.view.layer addSublayer:_colorLayer];
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [CATransaction begin];
    //关闭隐式动画
    CATransaction.disableActions = YES;
    CATransaction.animationDuration = 5;
    _colorLayer.backgroundColor = randomColor.CGColor;
    [CATransaction commit];
}

6.2.4  CGAffineTransform和CATransform3D分别有什么作用

//下面用例子来演示一个包含缩放、旋转和移动的变换，示例代码如下：

- (void)viewDidLoad {
    [super viewDidLoad];
    _searchImageView = [[UIImageView alloc]initWithFrame:CGRectMake(([UIScreen mainScreen]. bounds.size.width - 200)*0.5, ([UIScreen mainScreen].bounds.size.height - 200)*0.5,200, 200)];
    _searchImageView.image = [UIImage imageNamed:@"马"];
    [self.view addSubview:_searchImageView];
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    /* 创建一个空的CGAffineTransform */
    CGAffineTransform transform = CGAffineTransformIdentity;
    /* 缩放 */
    transform = CGAffineTransformScale(transform, 0.5, 0.5);
    /* 旋转 */
    transform = CGAffineTransformRotate(transform, M_PI_4);
    /* 平移 */
    transform = CGAffineTransformTranslate(transform, 100, 0);
    _searchImageView.transform = transform;
}


//通过CATransform3D中的m34元素来控制3D变换的透视效果：

- (void)viewDidLoad {
    [super viewDidLoad];
    _searchImageView = [[UIImageView alloc]initWithFrame:CGRectMake(([UIScreen mainScreen]. bounds.size.width - 200)*0.5, ([UIScreen mainScreen].bounds.size.height - 200)*0.5,200, 200)];
    _searchImageView.image = [UIImage imageNamed:@"马"];
    [self.view addSubview:_searchImageView];
    /* 创建一个空的CATransform3D */
    CATransform3D transform = CATransform3DIdentity;
    /* 设置m34元素，增强透视效果 */
    transform.m34 = -1.0/500.0;
    /* 3D旋转变换 */
    transform = CATransform3DRotate(transform, M_PI_4, 0, 1, 0);
    _searchImageView.layer.transform = transform;
}


6.2.6  如何使用UIView动画自定义过渡动画

- (void)startCustomTransitionAnimation {
    /* 获取截图 */
    UIGraphicsBeginImageContextWithOptions(self.view.bounds.size, YES, 0);
    [self.view.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *coverImage = UIGraphicsGetImageFromCurrentImageContext();
    /* 将截图添加到主视图 */
    UIImageView *coverImageView = [[UIImageView alloc]initWithImage:coverImage];
    coverImageView.frame = self.view.bounds;
    [self.view addSubview:coverImageView];
    /* 更换背景图片 */
    self.view.layer.contents = (id)[UIImage imageNamed:@"2"].CGImage;
    /* 添加动画 */
    [UIView animateWithDuration:1 animations:^{
        CGAffineTransform transform = CGAffineTransformMakeScale(0.01, 0.01);
        transform = CGAffineTransformRotate(transform, M_PI_2);
        coverImageView.alpha = 0;
        coverImageView.transform = transform;
    } completion:^(BOOL finished) {
        [coverImageView removeFromSuperview];
    }];
}


6.2.7  如何理解并使用CAKeyframeAnimation

- (void)startKeyframeAnimation {
    CAKeyframeAnimation *animation = [CAKeyframeAnimation animation];
    /* 指定需要做动画的属性 */
    animation.keyPath = @“backgroundColor";
    /* 指定关键帧 */
    animation.values = @[(id)[UIColor whiteColor].CGColor,(id)[UIColor redColor].CGColor,(id)(id) [UIColor blueColor].CGColor,(id)[UIColor blackColor].CGColor,(id)[UIColor whiteColor].CGColor];
    /* 指定关键帧的过渡时间 */
    animation.keyTimes = @[@0,@0.2,@0.5,@0.7,@1];
    animation.duration = 5;
    /* 指定关键帧的过渡缓冲方式 */
    animation.timingFunctions = @[[CAMediaTimingFunction functionWithName:kCAMediaTiming FunctionEaseInEaseOut],[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut], [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut], [CAMediaTiming Function functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
    [self.view.layer addAnimation:animation forKey:nil];
}

6.2.8  如何自定义UIViewController之间的转场动画

/* 设置transitioningDelegate为当前控制器 */
- (void)startCustomControllerTransitionAniamtion1:(UITouch *)touch {
    _touchPoint = [touch locationInView:self.view];
    TouchExampleController *vc = [TouchExampleController new];
    vc.type = 3;
    vc.transitioningDelegate = self;
    [self presentViewController:vc animated:YES completion:nil];
}
/* 当modal时，系统会自动调用此方法返回一个遵守UIViewControllerAnimatedTransitioning协议的对象 */
- (nullable id <UIViewControllerAnimatedTransitioning>)animationControllerForPresentedController: (UIViewController *)presented presentingController:(UIViewController *)presenting sourceController:(UIV iewController *)source {
    TransitionAnimator *animator = [TransitionAnimator new];
    /* 将单击坐标传过去，以确定动画开始的点 */
    animator.touchPoint = _touchPoint;
    return animator;
}
/* 在TransitionAnimator类实现中，实现UIViewControllerAnimatedTransitioning协议*/
@implementation TransitionAnimator
/* 设置动画时长 */
- (NSTimeInterval)transitionDuration:(nullable id <UIViewControllerContextTransitioning>) transitionContext {
    return 1;
}
/* 设置动画 */
- (void)animateTransition:(id <UIViewControllerContextTransitioning>)transitionContext {
    /* 从上下文中获取必需信息 */
    UIView *fromV = [transitionContext viewForKey:
                     UITransitionContextFromViewKey];
    UIView *toV = [transitionContext viewForKey:UITransitionContextToViewKey];
    UIView *containerView = [transitionContext containerView];
    /* 将视图添加到containerView中 */
    [containerView addSubview:fromV];
    [containerView addSubview:toV];
    CGFloat screenWidth = [UIScreen mainScreen].bounds.size.width;
    CGFloat screenHeight = [UIScreen mainScreen].bounds.size.height;
    CGPoint tempPoint = [containerView convertPoint:self.touchPoint fromView:[transitionContext viewForKey:UITransitionContextFromViewKey]];
    CGRect rect = CGRectMake(tempPoint.x-1, tempPoint.y-1, 2, 2);
    /* 使用UIBerizerPath绘制动画的开始路径和结束路径 */
    UIBezierPath *startPath = [UIBezierPath bezierPathWithOvalInRect:rect];
    UIBezierPath *endPath = [UIBezierPath bezierPathWithArcCenter:containerView.center radius:sqrt (screenHeight * screenHeight + screenWidth * screenWidth) startAngle:0 endAngle:M_PI*2 clockwise:YES];
    /* 设置maskLayer */
    CAShapeLayer *maskLayer = [CAShapeLayer layer];
    maskLayer.path = endPath.CGPath;
    toV.layer.mask = maskLayer;
    /* 设置动画 */
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"path"];
    animation.delegate = self;
    animation.fromValue = (id)startPath.CGPath;
    animation.toValue = (id)endPath.CGPath;
    animation.duration = [self transitionDuration:transitionContext];
    [maskLayer addAnimation:animation forKey:@"custom"];
    callback = ^ {
        toV.layer.mask = nil;
        /* 动画结束后一定要调用此方法完成过渡动画 */
        [transitionContext completeTransition:![transitionContext transitionWasCancelled]];
    };
}
- (void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag {
    callback();
}


6.2.9  如何保持视图界面为动画结束时的状态

/* 设置maskLayer */
CAShapeLayer *maskLayer = [CAShapeLayer layer];
maskLayer.path = startPath.CGPath;
toV.layer.mask = maskLayer;
/* 设置动画 */
CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"path"];
animation.delegate = self;
animation.fromValue = (id)startPath.CGPath;
animation.toValue = (id)endPath.CGPath;
animation.duration = [self transitionDuration:transitionContext];
animation.removedOnCompletion = NO;
animation.fillMode = kCAFillModeForwards;
[maskLayer addAnimation:animation forKey:@"custom"];
callback = ^ {
    toV.layer.mask = nil;
    /* 移除动画 */
    [maskLayer removeAnimationForKey:@"custom"];
    [transitionContext completeTransition:![transitionContext transitionWasCancelled]];
};


第7章  iOS中的数据持久化

//2.Property list文件存储：

/* 1.读取工程中的plist文件，这里设置的工程的plist是一个Dictionary，也可以用Array plist */
NSMutableDictionary *data = [[NSMutableDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"test" ofType:@"plist"]];
NSLog(@"从工程plist读取的数据:%@",data);

/* 添加新数据到字典对象中 */
[data setObject:@"4444" forKey:@"d"];
NSLog(@"将要写入沙盒的数据:%@",data);

/* 获取沙盒路径,这里"/demo.plist"是指新建的沙盒里plist文件路径，一定要加“/”!!! */
NSString *filePath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUser DomainMask,YES) lastObject] stringByAppendingPathComponent:@"/demo.plist"];
/* 2.将更新了的工程中的数据写入沙盒 */
[data writeToFile:filePath atomically:YES];

/* 3.读取沙盒数据 */
NSMutableDictionary *dic = [[NSMutableDictionary alloc]
                            initWithContentsOfFile:filePath];
NSLog(@"从沙盒读取的数据:%@",dic);

//3.Archive（归档）

//单一对象归档：

/* 沙盒根目录 */
NSString *homeDictory = NSHomeDirectory();
/* 拼接根目录和自定义文件名构建存储路径 */
NSString *homePath = [homeDictory stringByAppendingPathComponent:@"testAchiver"];
/* 待归档的数组，也可归档NSString、NSInteger等基础对象类型 */
NSArray *array = @[@"obj1",@"obj2",@"obj3"];
/* 归档并返回归档是否成功 */
BOOL flag = [NSKeyedArchiver archiveRootObject:array toFile:homePath];
NSArray *unarchivedArray;
/* 如果归档成功，那么解归档 */
if (flag) {
    unarchivedArray = [NSKeyedUnarchiver unarchiveObjectWithFile:homePath];
}
NSLog(@"%@",unarchivedArray);


//多对象归档：

/* 归档路径 */
NSString *homeDictory = NSHomeDirectory();
NSString *homePath = [homeDictory stringByAppendingPathComponent:@"testAchiver"];
/* 待归档的数据 */
NSInteger intVal = 1;
NSString *strVal = @"string";
CGPoint pointVal = CGPointMake(1.0, 1.0);
/* 初始化归档工具 */
NSMutableData *mulData = [[NSMutableData alloc] init];
NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc]initForWritingWithMutableData:mulData];
/* 归档 */
[archiver encodeInteger:intVal forKey:@"key_integer"];
[archiver encodeObject:strVal forKey:@"key_string"];
[archiver encodePoint:pointVal forKey:@"key_point"];
[archiver finishEncoding];
/* 归档的数据写入 */
[mulData writeToFile:homePath atomically:YES];
/* 解归档 */
NSMutableData *undata = [[NSMutableData alloc]initWithContentsOfFile:homePath];
NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc]initForReadingWithData:undata];
CGPoint unpointVal = [unarchiver decodePointForKey:@"key_point"];
NSString *unstrVal = [unarchiver decodeObjectForKey:@"key_string"];
NSInteger unintegerVal = [unarchiver decodeIntegerForKey:@"key_integer"];
[unarchiver finishDecoding];
NSLog(@"%f,%f,%@,%ld",unpointVal.x,unpointVal.y,unstrVal,(long)unintegerVal);


7.1  沙盒机制

/* 获取Documents目录 */
NSString *documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUser DomainMask, YES) lastObject];

/* 获取Documents目录 */
NSString *documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUser DomainMask, YES) lastObject];

/* 获取Documents目录 */
NSString *documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUser DomainMask, YES) lastObject];

获取各种沙盒路径的方法如下：
/* 1.获取Home目录路径： */
NSString *homeDir = NSHomeDirectory();
/* 2.获取Documents目录路径： */
NSArray *docpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUser Domain Mask, YES);
NSString *docDir = [docpaths objectAtIndex:0];
/* 3.获取Library/Caches目录路径： */
NSArray *cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUser Domain Mask, YES);
NSString *cachesDir = [cachepaths objectAtIndex:0];
/* 4.获取tmp目录路径： */
NSString *tmpDir = NSTemporaryDirectory();
/* 5.获取应用程序包中资源文件路径(获取程序包中一个图片资源'img.png'的路径)： */
NSString *imgPath = [[NSBundle mainBundle] pathForResource:@"img" ofType:@"png"];
UIImage *image = [[UIImage alloc] initWithContentsOfFile:imgPath];



7.2.2  如何使用NSUserDefault偏好设置保存数据

NSUserDefaults *userDefault = [NSUserDefaults standardUserDefaults];


//一般的存取操作：
- (void)userDefaultSave {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:@"jack" forKey:@"name"];
    [defaults setInteger:10 forKey:@"age"];
    /* 下面的代码可以省略，这里只是作为演示 */
    if ([defaults synchronize]) {
        NSLog(@"保存成功");
    }
}
- (void)userDefaultGet {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *name = [defaults objectForKey:@"name"];
    NSInteger age = [defaults integerForKey:@"age"];
    NSLog(@"name = %@, age = %zd",name,age);
}


// 如何保存UIImage对象：
- (void)userDefaultSaveImage {
    UIImage *img = [UIImage imageNamed:@"1"];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    /* 直接保存将产生错误：[defaults setObject:img forKey:@"img"];*/
    /* 转为NSData对象 */
    NSData *data = UIImageJPEGRepresentation(img, 1);
    [defaults setObject:data forKey:@"img"];
}
- (void)userDefaultGetImage {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    /* 转为NSData对象 */
    NSData *data = [defaults objectForKey:@"img"];
    /* 转为UIImage对象 */
    UIImage *img = [UIImage imageWithData:data];
    self.view.layer.contents = (id)img.CGImage;
}


7.2.3  如何使用NSUserDefault保存自定义对象

- (void)archiver1 {
    Animal *person = [Animal new];
    person.name = @"Jack";
    person.age = 30;
    
    /* 归档 */
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:person];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:data forKey:@"person"];
}

- (void)unarchiver1 {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *data = [defaults objectForKey:@"person"];
    /* 解档 */
    id obj = [NSKeyedUnarchiver unarchiveObjectWithData:data];
    NSLog(@"%@", obj);
}


7.2.4  什么是序列化或者归档

//首选需要在自定义的类中遵守NSCoding协议并实现编码和解码的方法。

/* 遵守协议 */
@interface Animal : NSObject<NSCoding>
/* 编码 */
- (void)encodeWithCoder:(NSCoder *)aCoder{
    [aCoder encodeObject:self.name forKey:@"name"];
    [aCoder encodeInteger:self.age forKey:@"age"];
}
/* 解码 */
- (nullable instancetype)initWithCoder:(NSCoder *)aDecoder {
    self.name = [aDecoder decodeObjectForKey:@"name"];
    self.age = [aDecoder decodeIntegerForKey:@"age"];
    return self;
}

//然后使用NSKeyedArchiver类对自定义对象进行归档，系统会自动调用编码方法。

- (void)archiver {
    Animal *person = [Animal new];
    person.name = @"Jack";
    person.age = 30;
    
    /* 获取文件路径 */
    NSString *documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUser DomainMask, YES) lastObject];
    NSString *path = [documents stringByAppendingPathComponent:@"person.archiver"];
    /* 归档 */
    [NSKeyedArchiver archiveRootObject:person toFile:path];
}

//此时在Documents目录下就会生成person.archiver文件，其中保存了编码之后的数据。接下来只需要在适当的时候将保存好的文件反归档为对象。

- (void)unarchiver {
    /* 获取文件路径 */
    NSString *documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUser DomainMask, YES) lastObject];
    NSString *path = [documents stringByAppendingPathComponent:@"person.archiver"];
    /* 解档 */
    id obj = [NSKeyedUnarchiver unarchiveObjectWithFile:path];
    NSLog(@"%@", obj);
}


8.2.1  当使用block时，什么情况会发生引用循环？如何解决

/* Test.h */
#import <Foundation/Foundation.h>
/* 声明一个名为MYBlock的block，参数为空，返回值为void */
typedef void (^MYBlock)();
@interface Test : NSObject
/* 定义并强引用一个MYBlock */
@property (nonatomic, strong) MYBlock block;
/* 对象属性 */
@property (nonatomic, copy) NSString *name;
- (void)print;
@end
/* Test.m */
#import "Test.h"
@implementation Test
- (void)print {
    self.block = ^{
        NSLog(@"%@",self.name);
    };
    self.block();
}
@end


//将对象使用__weak或者__block修饰符修饰之后再在block中使用（注意是在自动引用计数下），代码如下：

- (void)print {
    __weak typeof(self) weakSelf = self;
    self.block = ^{
        NSLog(@"%@",weakSelf.name);
    };
    self.block();
}


9.1  iOS网络编程与多线程基础

// 1．NSThread

- (void)viewDidLoad {
    [super viewDidLoad];
    /** NSThread静态工具方法 **/
    /* 1 是否开启了多线程 */
    BOOL isMultiThreaded = [NSThread isMultiThreaded];
    /* 2 获取当前线程 */
    NSThread *currentThread = [NSThread currentThread];
    /* 3 获取主线程 */
    NSThread *mainThread = [NSThread mainThread];
    /* 4 睡眠当前线程 */
    /* 4.1 线程睡眠5s */
    [NSThread sleepForTimeInterval:5];
    /* 4.2 线程睡眠到指定时间，效果同上 */
    [NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:5]];
    /* 5 退出当前线程，注意不要在主线程调用，防止主线程被kill掉 */
    //[NSThread exit];
    
    /* NSThread线程对象的基本创建，target为入口方法所在的对象，selector为线程入口方法 */
    /* 1 线程实例对象创建与设置 */
    NSThread *newThread= [[NSThread alloc] initWithTarget:self selector:@selector(run) object:nil];
    /* 设置线程优先级threadPriority(0~1.0)，该属性即将被抛弃，将使用qualityOfService代替 */
    // newThread.threadPriority = 1.0;
    newThread.qualityOfService = NSQualityOfServiceUserInteractive;
    /* 开启线程 */
    [newThread start];
    /* 2 静态方法快速创建并开启新线程 */
    [NSThread detachNewThreadSelector:@selector(run) toTarget:self withObject:nil];
    [NSThread detachNewThreadWithBlock:^{
        NSLog(@"block run...");
    }];
    
    /** NSObejct基类隐式创建线程的一些静态工具方法 **/
    /* 1 在当前线程上执行方法，延迟2s */
    [self performSelector:@selector(run) withObject:nil afterDelay:2.0];
    /* 2 在指定线程上执行方法，不等待当前线程 */
    [self performSelector:@selector(run) onThread:newThread withObject:nil waitUntilDone:NO];
    /* 3 后台异步执行方法 */
    [self performSelectorInBackground:@selector(run) withObject:nil];
    /* 4 在主线程上执行方法 */
    [self performSelectorOnMainThread:@selector(run) withObject:nil waitUntilDone:NO];
}
- (void)run {
    NSLog(@"run...");
}


// 2．GCD

//（2）同步和异步任务派发（Synchronous和Asynchronous）：
/* 1. 提交异步任务 */
NSLog(@"开始提交异步任务:");
dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_ PRIORITY_DEFAULT, 0), ^{
    /* 耗时任务... */
    [NSThread sleepForTimeInterval:10];
});
NSLog(@"异步任务提交成功！");

/* 2. 提交同步任务 */
NSLog(@"开始提交同步任务:");
dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_ PRIORITY_DEFAULT, 0), ^{
    /* 耗时任务... */
    [NSThread sleepForTimeInterval:10];
});
NSLog(@"同步任务提交成功！");


//（3）dispatch_queue_t
/* 创建一个并发队列 */
dispatch_queue_t concurrent_queue = dispatch_queue_create("demo.gcd.concurrent_queue", DISPATCH_ QUEUE_CONCURRENT);
/* 创建一个串行队列 */
dispatch_queue_t serial_queue = dispatch_queue_create("demo.gcd.serial_queue", DISPATCH_QUEUE_ SERIAL);


/* 获取主队列（在主线程上执行）*/
dispatch_queue_t main_queue = dispatch_get_main_queue();
/* 获取不同优先级的全局队列(优先级从高到低) */
dispatch_queue_t queue_high = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
dispatch_queue_t queue_default = dispatch_get_global_queue (DISPATCH_QUEUE_PRIORITY_ DEFAULT, 0);
dispatch_queue_t queue_low = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0);
dispatch_queue_t queue_background = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_ BACKGROUND, 0);


//（4）dispatch_once_t
+ (instancetype)sharedInstance {
    static dispatch_once_t once = 0;
    static id sharedInstance = nil;
    dispatch_once(&once, ^{
        /* 只实例化一次 */
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}


//（5）dispatch_after：
/* 定义延迟时间:3s */
dispatch_time_t delay = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC));
dispatch_after(delay, dispatch_get_main_queue(), ^{
    // 要执行的任务...
});

//（6）dispatch_group_t：
dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
dispatch_group_t group = dispatch_group_create();
dispatch_group_async(group, queue, ^{ /*操作1 */ });
dispatch_group_async(group, queue, ^{ /*操作2 */ });
dispatch_group_async(group, queue, ^{ /*操作3 */ });
dispatch_group_notify(group, dispatch_get_main_queue(), ^{
    // 后续操作...
});

//（7）同步代码到主线程

dispatch_async(dispatch_get_main_queue(), ^{
    // UI更新代码...
});


3．NSOperation

//它们的简单用法如下：
- (void)viewDidLoad {
    [super viewDidLoad];
    /* NSInvocationOperation初始化 */
    NSInvocationOperation *invoOpertion = [[NSInvocationOperation alloc] initWithTarget:self selector:@selector(run) object:nil];
    [invoOpertion start];
    
    /* NSBlockOperation初始化 */
    NSBlockOperation *blkOperation = [NSBlockOperation blockOperationWithBlock:^{
        NSLog(@"NSBlockOperation");
    }];
    [blkOperation start];
}

- (void)run {
    NSLog(@"NSInvocationOperation");
}

//另外，NSBlockOperation可以后续继续添加block执行块，操作启动后会在不同线程并发地执行这些执行块：
/* NSBlockOperation初始化 */
NSBlockOperation *blkOperation = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"NSBlockOperationA %@", [NSThread currentThread]);
}];
[blkOperation addExecutionBlock:^{
    NSLog(@"NSBlockOperationB %@", [NSThread currentThread]);
}];
[blkOperation addExecutionBlock:^{
    NSLog(@"NSBlockOperationC %@", [NSThread currentThread]);
}];
[blkOperation start];


//设置各操作之间的依赖实现线程同步：

/* 获取主队列(主线程) */
NSOperationQueue *queue = [NSOperationQueue mainQueue];
/* 创建a、b、c操作 */
NSOperation *c = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"OperationC");
}];
NSOperation *a = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"OperationA");
}];
NSOperation *b = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"OperationB");
}];
/* 添加操作依赖，c依赖于a和b，这样c一定会在a和b完成后才执行，即顺序为：A、B、C */
[c addDependency:a];
[c addDependency:b];
/* 添加操作a、b、c到操作队列queue(特意将c在a和b之前添加) */
[queue addOperation:c];
[queue addOperation:a];
[queue addOperation:b];


9.1.5  UIKit类要在哪一个应用线程上使用

//例如下面的问题代码：

@interface TTWaitController : UIViewController
@property (strong, nonatomic) UILabel *alert;
@end

@implementation TTWaitController
- (void)viewDidLoad
{
    CGRect frame = CGRectMake(20, 200, 200, 20);
    self.alert = [[UILabel alloc] initWithFrame:frame];
    self.alert.text = @"Please wait 10 seconds...";
    self.alert.textColor = [UIColor whiteColor];
    [self.view addSubview:self.alert];
    NSOperationQueue *waitQueue = [[NSOperationQueue alloc] init];
    [waitQueue addOperationWithBlock:^{
        [NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:10]];
        self.alert.text = @"Thanks!";
    }];
}
@end

@implementation TTAppDelegate
- (BOOL)application:(UIApplication *)application
didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [[TTWaitController alloc] init];
    [self.window makeKeyAndVisible];
    return YES;
}


9.1.6  iOS中有哪几种从其他线程回到主线程的方法

//1．NSThreadPerformAdditions协议：

-(void)performSelectorOnMainThread:(SEL)aSelector withObject:
(nullable id)arg waitUntilDone:(BOOL)wait modes:(nullable NSArray<NSString *> *)array;
- (void)performSelectorOnMainThread:(SEL)aSelector withObject:
(nullable id)arg waitUntilDone:(BOOL)wait;

//2．GCD

dispatch_async(dispatch_get_main_queue(), ^{
});

//3．NSOperationQueue

//示例代码如下：
- (void)goToMainThread {
    /* 开启子线程下载图片 */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        NSData *imageData = [[NSData alloc]initWithContentsOfURL:[NSURL URLWithString: @"http://jxh1992.com/pe0060886.jpg"]];
        _image = [UIImage imageWithData:imageData];
        /* 切换到主线程显示 */
        //1.NSThread
        //[self performSelectorOnMainThread:@selector(changeBg) withObject:nil waitUntilDone:NO];
        //2.GCD
        //dispatch_async(dispatch_get_main_queue(), ^{
        //[self changeBg];
        //});
        //3.NSOperationQueue
        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
            [self changeBg];
        }];
    });
}


9.1.7  用户下载一个大图片，分成很多份下载，如何使用GCD实现

dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
dispatch_group_t group = dispatch_group_create();
dispatch_group_async(group, queue, ^{ /*加载图片1 */ });
dispatch_group_async(group, queue, ^{ /*加载图片2 */ });
dispatch_group_async(group, queue, ^{ /*加载图片3 */ });
dispatch_group_notify(group, dispatch_get_main_queue(), ^{
    // 合并图片……
});


9.1.9  NSOpertion如何实现线程依赖

/* 获取主队列(主线程) */
NSOperationQueue *queue = [NSOperationQueue mainQueue];
/* 创建a、b、c操作 */
NSOperation *a = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"Operation A Start!");
    [NSThread sleepForTimeInterval:3.0];
    NSLog(@"Operation A Done!");
}];
NSOperation *b = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"Operation B Start!");
    [NSThread sleepForTimeInterval:3.0];
    NSLog(@"Operation B Done!");
}];
NSOperation *c = [NSBlockOperation blockOperationWithBlock:^{
    NSLog(@"Operation C Start!");
    // ... ...
}];
/* 添加操作依赖，c依赖于a和b */
[c addDependency:a];
[c addDependency:b];
/* 添加操作a、b、c到操作队列queue(特意将c在a和b之前添加) */
[queue addOperation:c];
[queue addOperation:a];
[queue addOperation:b];


9.1.11  dispatch_barrier_(a)sync的作用是什么

/* 创建并发队列 */
dispatch_queue_t concurrentQueue = dispatch_queue_create("test.concurrent.queue", DISPATCH_ QUEUE_CONCURRENT);
/* 添加两个并发操作A和B，即A和B会并发执行 */
dispatch_async(concurrentQueue, ^(){
    NSLog(@"OperationA");
});
dispatch_async(concurrentQueue, ^(){
    NSLog(@"OperationB");
});
/* 添加barrier障碍操作，会等待前面的并发操作结束，并暂时阻塞后面的并发操作直到其完成 */
dispatch_barrier_async(concurrentQueue, ^(){
    NSLog(@"OperationBarrier!");
});
/* 继续添加并发操作C和D，要等待barrier障碍操作结束才能开始 */
dispatch_async(concurrentQueue, ^(){
    NSLog(@"OperationC");
});
dispatch_async(concurrentQueue, ^(){
    NSLog(@"OperationD");
});


9.1.12  如何理解线程安全

//1．@synchronized关键字

- (void)testSynchronized {
    Animal *someone = [Animal new];
    /* 线程A */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        @synchronized (someone) {
            NSLog(@"线程A=%@",[NSThread currentThread]);
            someone.name = @" ";
            [NSThread sleepForTimeInterval:5];
        }
    });
    /* 线程B */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        @synchronized (someone) {
            NSLog(@"线程B=%@",[NSThread currentThread]);
            someone.name = @" ";
        }
    });
}


//2．NSLock

- (void)testNSLock {
    Animal *someone = [Animal new];
    /* 创建锁对象 */
    NSLock *alock = [[NSLock alloc]init];
    /* 线程A */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        /* 尝试上锁 */
        if ([alock tryLock]) {//[alock lock];
            NSLog(@"线程A=%@",[NSThread currentThread]);
            someone.name = @"🕊";
            [NSThread sleepForTimeInterval:5];
            /* 开锁 */
            [alock unlock];
        }
    });
    /* 线程B */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        if ([alock tryLock]) {// [alock lock];
            NSLog(@"线程B=%@",[NSThread currentThread]);
            someone.name = @"🐰";
            [alock unlock];
        }
    });
}

// 3．GCD

- (void)testGCD {
    Animal *someone = [Animal new];
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(1);
    /* 线程A */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        /* 等待，如果信号大于0，那么继续并减少一个信号 */
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"线程A=%@",[NSThread currentThread]);
        someone.name = @"🕊";
        [NSThread sleepForTimeInterval:5];
        /* 信号加1 */
        dispatch_semaphore_signal(semaphore);
    });
    /* 线程B */
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"线程B=%@",[NSThread currentThread]);
        someone.name = @"🐰";
        dispatch_semaphore_signal(semaphore);
    });
}


9.1.14  如何使用NSURLConnection进行网络请求

//（1）同步GET请求方法
//示例代码如下：

- (void)connectionSyncGet {
    /* 创建NSURL对象 */
    NSURL *url = [NSURL URLWithString:IMAGEURL];
    /* 创建请求对象，默认为GET请求 */
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    /* 创建请求响应对象 */
    NSHTTPURLResponse *response = nil;
    NSError *error = nil;
    /* 发送请求，同步请求会阻塞当前线程 */
    NSData *data = [NSURLConnection sendSynchronousRequest:request returningResponse: &response error:&error];
    /* 解析返回的数据 */
    _image = [UIImage imageWithData:data];
    /* 显示图片 */
    [self changeBg];
}
- (void)changeBg {
    self.view.layer.contents = (id)_image.CGImage;
}

//（2）异步POST请求方法
// 代码如下：

- (void)connectionAsyncPost {
    NSURL *url = [NSURL URLWithString:IMAGEURL];
    /* 创建请求对象 */
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    /* 设置请求方式为POST */
    request.HTTPMethod = @"POST";
    /* 设置请求体，在请求体中设置参数 */
    request.HTTPBody = [@"username=520it&pwd=520&type=JSON"
                        dataUsingEncoding:NSUTF8StringEncoding];
    /* 设置请求超时 */
    request.timeoutInterval = 15;
    /* 设置请求头 */
    [request setValue:@"iOS" forHTTPHeaderField:@"User-Agent"];
    /* 发送请求 */
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue
                                                            mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData *
                                                                                           _Nullable data, NSError * _Nullable connectionError) {
        NSLog(@"NSThread = %@",[NSThread currentThread]);
        if (connectionError) {
            NSLog(@"error = %@",connectionError.userInfo);
        }else {
            /* 解析返回的数据 */
            _image = [UIImage imageWithData:data];
            /* 显示图片 */
            [self changeBg];
        }
    }];
}

//（3）NSURLConnectionDelegate的使用
// 可以使用NSURLConnectionDelegate监听网络请求的响应。示例代码如下：

- (void)connectGETDelegate {
    /* 创建NSURL对象 */
    NSURL *url = [NSURL URLWithString:IMAGEURL];
    /* 创建请求对象，默认为GET请求 */
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    /* 设置代理 */
    [NSURLConnection connectionWithRequest:request delegate:self];
}

/* 当接收到服务器响应的时候调用。第一个参数connection：监听的是哪个
 NSURLConnection对象；第二个参数response：接收到的服务器返回的响应头信息 */
-(void)connection:(nonnull NSURLConnection *)connection didReceiveResponse:(nonnull NSURLResponse *)response {
    _imageData = [NSMutableData data];
}
/* 当接收到数据的时候调用，该方法会被调用多次。第一个参数connection：监听的
 是哪个NSURLConnection对象；第二个参数data：本次接收到的服务端返回的二进制数
 据（可能是片段）*/
- (void)connection:(nonnull NSURLConnection *)connection didReceiveData:(nonnull NSData *)data {
    [_imageData appendData:data];
}
/* 服务端返回的数据接收完毕之后会调用。通常在该方法中解析服务器返回的数据*/
-(void)connectionDidFinishLoading:(nonnull NSURLConnection *)connection {
    /* 解析返回的数据 */
    _image = [UIImage imageWithData:_imageData];
    /* 显示图片 */
    [self changeBg];
}
/* 当请求错误的时候调用（例如请求超时）。第一个参数connection：NSURLConnection
 对象。第二个参数：网络请求的错误信息，如果请求失败，那么error有值 */
- (void)connection:(nonnull NSURLConnection *)connection didFailWithError:(nonnull NSError *)error {
}


9.1.15  如何使用NSURLSession进行网络请求

- (void)sessionGet {
    /* 创建NSURL对象 */
    NSURL *url = [NSURL URLWithString:IMAGEURL];
    /* 创建请求对象，默认为GET请求 */
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    /* 创建配置 */
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
    /* 创建NSURLSession对象 */
    NSURLSession *session = [NSURLSession sessionWithConfiguration:config];
    /* 创建任务 */
    NSURLSessionDataTask *task = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error)
                                  {
                                      if (error) {
                                          return ;
                                      }
                                      /* 解析返回的数据 */
                                      _image = [UIImage imageWithData:data];
                                      /* 显示图片，注意！此时是异步线程需要在主线程中显示图片 */
                                      [self changeBg];
                                  }];
    [task resume];
}


9.2.2  在ARC环境下，是否需要使用copy关键字来修饰block

-(void)test {
    void (^gBlock1)(int , int ) =^(int a, int b){
        NSLog(@"a + b = %d", a+b);
    };
    NSLog(@"%@", gBlock1);
}


9.2.3  在block内如何修改block外部变量

/* block外部变量 */
__block int var1 = 0;
int var2 = 0;
/* 定义block */
void (^block)(void) = ^{
    /* 试图修改block外部变量 */
    var1 = 100;
    /* 编译错误，在block内部不可对var2赋值 */
    // var2 = 1;
};
/* 执行block */
block();
NSLog(@"修改后的var1:%d",var1); // 修改后的var1:100


int var = 1;
void block(){
    int val = var;
    val += 1;
}


__block int var = 1;
void block(){
    int *ptr = &var;
    *ptr += 1;
}


9.2.4  在block中使用self关键字是否一定导致循环引用

//示例代码：

typedef void(^block)();
@property (copy, nonatomic) block myBlock;
@property (copy, nonatomic) NSString *blockString;
- (void)testBlock {
    self.myBlock = ^() {
        /* 其实注释中的代码，同样会造成循环引用 */
        NSString *localString = self.blockString;
    };
}


// 使用弱引用（weak reference）的方法来避免循环引用的产生：

typedef void(^block)();
@property (copy, nonatomic) block myBlock;
@property (copy, nonatomic) NSString *blockString;
-(void)testBlock {
    __weak typeOf(self) weakSelf = self;
    self.myBlock = ^() {
        /* 其实注释中的代码，同样会造成循环引用 */
        NSString *localString = weakSelf.blockString;
    };
}


//不会造成循环引用的几种情况：

/* 使用GCD异步执行主队列任务 */
dispatch_async(dispatch_get_main_queue(), ^{
    [self doSomething];
});


- (void)testBlock {
    __weak typeOf(self) weakSelf = self;
    self.myBlock = ^() {
        __strong typeOf(self) strongSelf = weakSelf;
        NSString *localString = strongSelf.blockString;
    };
}


9.2.7  如何使用GCD实现线程之间的通信

//示例代码如下：

dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    /* 执行耗时操作 */
    for (int i = 0; i<10000; i++) {
        NSLog(@"i = %i",i);
    }
    /* 回到主线程 */
    dispatch_async(dispatch_get_main_queue(), ^{
        //更新UI
    });
});


9.2.8  GCD如何实现线程同步

//引申1：使用信号量实现任务2依赖于任务1，即任务2要等待任务1结束才开始执行

/* 创建一个信号量 */
dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
/* 任务1 */
dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    /* 耗时任务1 */
    NSLog(@"任务1开始");
    [NSThread sleepForTimeInterval:3];
    NSLog(@"任务1结束");
    /* 任务1结束，发送信号告诉任务2可以开始了 */
    dispatch_semaphore_signal(semaphore);
});
/* 任务2 */
dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    /* 等待任务1结束获得信号量, 无限等待 */
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    /* 如果获得信号量，那么开始任务2 */
    NSLog(@"任务2开始");
    [NSThread sleepForTimeInterval:3];
    NSLog(@"任务2结束");
});
[NSThread sleepForTimeInterval:10];


引申2：通过信号量控制最大并发数量

/* 创建一个信号量并初始化为5 */
dispatch_semaphore_t semaphore = dispatch_semaphore_create(5);
/* 模拟1000个等待执行的任务，通过信号量控制最大并发任务数量为5 */
for (int i = 0; i < 1000; i++) {
    /* 任务i */
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        /* 耗时任务1，执行前等待信号使信号量减1 */
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"任务%d开始", i);
        [NSThread sleepForTimeInterval:10];
        NSLog(@"任务%d结束", i);
        /* 任务i结束，发送信号释放一个资源 */
        dispatch_semaphore_signal(semaphore);
    });
}
[NSThread sleepForTimeInterval:1000];


9.2.9  GCD多线程编程中什么时候会创建新线程

/* 创建一个串行队列 */
dispatch_queue_t serialQueue = dispatch_queue_create("serial.queue", DISPATCH_QUEUE_SERIAL);
/* 创建一个并发队列 */
dispatch_queue_t concurrentQueue = dispatch_queue_create("concurrent.queue", DISPATCH_QUEUE_CONCURRENT);

//1）串行队列中提交同步任务：不会开启新线程，直接在当前线程同步地串行执行这些任务。
/* 1 串行队列添加同步任务：没有开启新线程，全部在主线程串行执行 */
dispatch_sync(serialQueue, ^{
    NSLog(@"SERIAL_SYN_A %@", [NSThread currentThread]);
});
dispatch_sync(serialQueue, ^{
    NSLog(@"SERIAL_SYN_B %@", [NSThread currentThread]);
});
dispatch_sync(serialQueue, ^{
    NSLog(@"SERIAL_SYN_C %@", [NSThread currentThread]);
});

//2）串行队列中提交异步任务:会开启一个新线程，在新子线程异步地串行执行这些任务。
/* 2 串行队列添加异步任务:开启了一个新子线程并共用，串行执行 */
dispatch_async(serialQueue, ^{
    NSLog(@"SERIAL_ASYN_A %@", [NSThread currentThread]);
});
dispatch_async(serialQueue, ^{
    NSLog(@"SERIAL_ASYN_B %@", [NSThread currentThread]);
});
dispatch_async(serialQueue, ^{
    NSLog(@"SERIAL_ASYN_C %@", [NSThread currentThread]);
});

//3）并发队列中提交同步任务：不会开启新线程，效果和“串行队列中提交同步任务”一样，直接在当前线程同步地串行执行这些任务。
/* 3 并发队列添加同步任务：没有开启新线程，全部在主线程串行执行 */
dispatch_sync(concurrentQueue, ^{
    NSLog(@"CONCURRENT_SYN_A %@", [NSThread currentThread]);
});
dispatch_sync(concurrentQueue, ^{
    NSLog(@"CONCURRENT_SYN_B %@", [NSThread currentThread]);
});
dispatch_sync(concurrentQueue, ^{
    NSLog(@"CONCURRENT_SYN_C %@", [NSThread currentThread]);
});

//4）并发队列中提交异步任务：会开启多个子线程，在子线程异步地并发执行这些任务。
/* 4 并发队列添加异步任务：开启多个子线程，并发执行多个任务 */
dispatch_async(concurrentQueue, ^{
    NSLog(@"CONCURRENT_ASYN_A %@", [NSThread currentThread]);
});
dispatch_async(concurrentQueue, ^{
    NSLog(@"CONCURRENT_ASYN_B %@", [NSThread currentThread]);
});
dispatch_async(concurrentQueue, ^{
    NSLog(@"CONCURRENT_ASYN_C %@", [NSThread currentThread]);
});


9.2.10  iOS中如何触发定时任务或延时任务

1．performSelector实现延时任务

/* 延迟10s执行任务 */
[self performSelector:@selector(task) withObject:nil afterDelay:10];
-（void）task
{
    // delay task
}

2．利用sleep实现后面任务的等待

[NSThread sleepForTimeInterval:10.0];


3．GCD实现延时或定时任务

//通过GCD实现block代码块的延时执行。
dispatch_time_t delay = dispatch_time(DISPATCH_TIME_NOW, 10 * NSEC_PER_SEC);
dispatch_after(delay, dispatch_get_main_queue(), ^{
    // delay task
});


//GCD还可以用来实现定时器功能，还能设置延时开启计时器，使用中注意一定要定义强引用指针来指向计时器对象才可让计时器生效。
/* 必须要用强引用指针，计时器才会生效 */
@property (nonatomic, strong)dispatch_source_t timer;

/* 在指定线程上定义计时器 */
dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
dispatch_source_t _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
/* 开始的时间 */
dispatch_time_t when = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.0 * NSEC_PER_SEC));
/* 设置计时器 */
dispatch_source_set_timer(_timer, when, 1.0 * NSEC_PER_SEC, 0);
/* 计时器回调block */
dispatch_source_set_event_handler(_timer, ^{
    NSLog(@"dispatch_source_set_timer is working!");
});
/* 开启计时器 */
dispatch_resume(_timer);
/* 强引用计时器对象 */
self.timer = _timer;


4．NSTimer实现定时任务

//以下代码为4种情形下NSTimer的正确使用方法：
- (void)viewDidLoad {
    [super viewDidLoad];
    /* 第一种，主线程中创建timer ，需要手动添加到RunLoop中*/
    NSTimer *mainThreadTimer = [NSTimer timerWithTimeInterval:10.0 target:self selector: @selector(mainThreadTimer_SEL) userInfo:nil repeats:YES];
    /*第二种，主线程中创建timer，不需要手动添加到RunLoop中*/
    NSTimer *mainThreadSchduledTimer = [NSTimer
                                        scheduledTimerWithTimeInterval:10.0 target:self selector:@selector(mainThreadSchduledTimer_SEL) userInfo:nil repeats:YES];
    /* 将mainThreadTimer添加到主线程runloop */
    [[NSRunLoop currentRunLoop] addTimer:mainThreadTimer
                                 forMode:NSDefaultRunLoopMode];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        /*第三种，子线程中创建timer ，需要手动添加到RunLoop中*/
        NSTimer *subThreadTimer = [NSTimer timerWithTimeInterval:10.0 target:self selector: @selector (subThreadTimer_SEL) userInfo:nil repeats:YES];
        /*第三种，子线程中创建timer ，不需要手动添加到RunLoop中*/
        NSTimer *subThreadSchduledTimer = [NSTimer scheduledTimerWithTimeInterval:10.0 target: self selector:@selector(subThreadSchduledTimer_SEL) userInfo:nil repeats:YES];
        /* 将subThreadTimer添加到子线程runloop */
        [[NSRunLoop currentRunLoop] addTimer:subThreadTimer forMode:NSDefaultRunLoopMode];
        /* 启动子线程runloop */
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    });
}
- (void) mainThreadTimer_SEL{
    NSLog(@"mainThreadTimer is working!");
}
- (void) mainThreadSchduledTimer_SEL{
    NSLog(@"mainThreadSchduledTimer is working!");
}
- (void) subThreadTimer_SEL{
    NSLog(@"subThreadTimer is working!");
}
- (void) subThreadSchduledTimer_SEL{
    NSLog(@"subThreadSchduledTimer is working!");
}

NSTimer的释放方法：[timer invalidate];


5．CADisplayLink实现定时任务

//下面是CADisplayLink定时器的实现：
- (void)viewDidLoad {
    [super viewDidLoad];
    CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector: @selector (displayLink_SEL)];
    /* 添加到当前运行的RunLoop中启动 */
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop]
                      forMode:NSDefaultRunLoopMode];
    /* 暂停、继续对selector的调用 */
    //[displayLink setPaused:YES];
    //[displayLink setPaused:NO];
    /* 设置每几帧调用一次selector，默认为1 */
    //[displayLink setPreferredFramesPerSecond:2];
    /* 移除，不再使用 */
    //[displayLink invalidate];
    //displayLink = nil;
}
- (void) displayLink_SEL{
    NSLog(@"displayLink is working!");
}



10.1.1  什么是ReactiveCocoa？如何使用

//下面简单地介绍RAC的使用。示例代码如下：

/* 1.1创建一个信号，并订阅它 */
RACSignal *signal = [RACSignal createSignal:^RACDisposable * _Nullable(id<RACSubscriber>  _Nonnull subscriber) {
    int a = 0;
    [subscriber sendNext:@(a)];
    [subscriber sendCompleted];
    return nil;
}];
/* 订阅信号signal */
[signal subscribeNext:^(id  _Nullable x) {
    NSLog(@"x:%@",x);
}];

/* 1.2监听一个UITextField属性text的变化，并打印text的值 */
[self.textField.rac_textSignal subscribeNext:^(NSString *text) {
    NSLog(@"text:%@",text);
}];

/* 1.3 使用filter过滤UITextField中text长度小于6的字符串 */
[[self.textField.rac_textSignal filter:^BOOL(NSString * _Nullable value) {
    return value.length > 6;
}] subscribeNext:^(NSString * text) {
    NSLog(@"text value:%@",text);
}];

/* 1.4通过map重新映射的信号持有的变量，实现获取UITextField中text的长度 */
RACSignal *textLengthSignal = [self.textField.rac_textSignal map:^id _Nullable(NSString * value) {
    return @(value.length);
}];
[textLengthSignal subscribeNext:^(NSNumber *length) {
    NSLog(@"text length:%d",[length intValue]);
}];

/* 1.5使用signalForControlEvents代替addTarget实现事件绑定 */
[[self.button rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext: ^(__kindof UIControl * _Nullable x) {
    // do something ...
    NSLog(@"button click");
}];
/* 1.6将某个对象的text属性绑定到UITextField的text上 */
RAC(self.model,text) = self.textField.rac_textSignal;

/* 1.7RAC遍历数组元素 */
NSArray *numbers = @[@1,@2,@3,@4];
[numbers.rac_sequence.signal subscribeNext:^(id x) {
    NSLog(@"%@",x);
}];


10.2.1  什么是单例模式

+ (instancetype)sharedInstance
{
    static id sharedInstance;
    @synchronized(self) {
        if (sharedInstance == nil) {
            sharedInstance = [[SingletonClass alloc] init];
        }
    }
    return sharedInstance;
}


/* SingletonClass.h*/
#import <Foundation/Foundation.h>
@interface SingletonClass : NSObject
/* 测试变量 */
@property (nonatomic, copy)NSString *name;
/* class单例 */
+ (instancetype *)sharedInstance;
@end
/* SingletonClass.m*/
#import "SingletonClass.h"
@implementation SingletonClass
/* class单例 */
+ (instancetype)sharedInstance {
    static dispatch_once_t once = 0;
    static id sharedInstance = nil;
    dispatch_once(&once, ^{
        /* 只实例化一次 */
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}
/* 测试 */
- (void)test {
    /* 可在整个工程中调用如下代码：*/
    [SingletonClass sharedInstance].name = @"sharedInstnce";
    NSString *name = [SingletonClass sharedInstance].name;
}
@end



































