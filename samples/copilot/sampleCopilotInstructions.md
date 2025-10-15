# PRIME DIRECTIVE
- Always try to cite source file name and line number for reference.

## MANDATORY PLANNING PHASE
	When working with large files (>300 lines) or complex changes:
		- ALWAYS start by creating a detailed plan BEFORE making any edits
        - Your plan MUST include:
            - All functions/sections that need modification
            - The order in which changes should be applied
            - Dependencies between changes
            - Estimated number of separate edits required
                
        - Format your plan as:
            - Working with: [filename]
            - Total planned edits: [number]

## MAKING EDITS
	- Focus on one conceptual change at a time
	- Show clear "before" and "after" snippets when proposing changes
	- Include concise explanations of what changed and why
	- Always check if the edit maintains the project's coding style

## EDIT SEQUENCE:
	1. [First specific change] - Purpose: [why]
	2. [Second specific change] - Purpose: [why]
	3. Do you approve this plan? I'll proceed with Edit [number] after your confirmation.
	4. WAIT for explicit user confirmation before making ANY edits when user ok edit [number]
            
## EXECUTION PHASE
	- After each individual edit, clearly indicate progress:
		"✅ Completed edit [#] of [total]. Ready for next edit?"
	- If you discover additional needed changes during editing:
	- STOP and update the plan
	- Get approval before continuing
                
## REFACTORING GUIDANCE
	When refactoring large files:
	- Break work into logical, independently functional chunks
	- Ensure each intermediate state maintains functionality
	- Consider temporary duplication as a valid interim step
	- Always indicate the refactoring pattern being applied

## CPP CODE STYLE
- Use 4 spaces for indentation, no tabs.