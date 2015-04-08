<?php
/* @var $this WSNkongyadataController */
/* @var $model WSNkongyadata */
/* @var $form CActiveForm */
?>

<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'wsnkongyadata-form',
	// Please note: When you enable ajax validation, make sure the corresponding
	// controller action is handling ajax validation correctly.
	// There is a call to performAjaxValidation() commented in generated controller code.
	// See class documentation of CActiveForm for details on this.
	'enableAjaxValidation'=>false,
)); ?>

	<p class="note">Fields with <span class="required">*</span> are required.</p>

	<?php echo $form->errorSummary($model); ?>

	<div class="row">
		<?php echo $form->labelEx($model,'DID'); ?>
		<?php echo $form->textField($model,'DID'); ?>
		<?php echo $form->error($model,'DID'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'SID'); ?>
		<?php echo $form->textField($model,'SID'); ?>
		<?php echo $form->error($model,'SID'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'DataTime'); ?>
		<?php echo $form->textField($model,'DataTime',array('size'=>50,'maxlength'=>50)); ?>
		<?php echo $form->error($model,'DataTime'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'S1'); ?>
		<?php echo $form->textField($model,'S1',array('size'=>50,'maxlength'=>50)); ?>
		<?php echo $form->error($model,'S1'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'S2'); ?>
		<?php echo $form->textField($model,'S2',array('size'=>50,'maxlength'=>50)); ?>
		<?php echo $form->error($model,'S2'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'R1'); ?>
		<?php echo $form->textField($model,'R1',array('size'=>50,'maxlength'=>50)); ?>
		<?php echo $form->error($model,'R1'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'R2'); ?>
		<?php echo $form->textField($model,'R2',array('size'=>50,'maxlength'=>50)); ?>
		<?php echo $form->error($model,'R2'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'IsWarning'); ?>
		<?php echo $form->textField($model,'IsWarning',array('size'=>50,'maxlength'=>50)); ?>
		<?php echo $form->error($model,'IsWarning'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->