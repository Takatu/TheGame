#pragma once

#include "Engine/DataTable.h"
#include "ExampleDataTable.generated.h"

/** Structure that defines an example data table entry */
USTRUCT(BlueprintType)

struct FExampleDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	/** Value01 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Example)
	int32 Value01;

	/** Value02 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Example)
	int32 Value02;

	/** Value03 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Example)
	int32 Value03;
};